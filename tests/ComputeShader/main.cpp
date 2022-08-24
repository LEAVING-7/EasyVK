#include <EasyVK/BufferAllocator.hpp>
#include <EasyVK/Command.hpp>
#include <EasyVK/Descriptor.hpp>
#include <EasyVK/Device.hpp>
#include <EasyVK/Instance.hpp>
#include <EasyVK/PipelineBuilder.hpp>
#include <EasyVK/Shader.hpp>
#include <EasyVK/SyncStructures.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
#include <cstdio>
#include <stdexcept>

u32 BUFFER_ELEMENTS = 10000;

VkDeviceSize bufferSize;
struct App {
  ezvk::Instance instance;
  ezvk::Device   device;

  VkQueue computeQueue;
  u32     computeQueueIndex;

  VkQueue transferQueue;
  u32     transferQueueIndex;

  ezvk::CommandPool cmdPool;

  ezvk::CommandBuffer cmdCompute;

  std::vector<u32> computeInput;
  std::vector<u32> computeOutput;

  ezvk::BufferAllocator allocator;
  ezvk::AllocatedBuffer deviceBuffer, hostBuffer;

  ezvk::DescriptorPool descriptorPool;

  VkPipeline computePipeline;

  void init() {
    instance.create({}, {}, "ComputeShaders");
    vkb::PhysicalDeviceSelector selector(instance);
    selector.set_minimum_version(1, 1).require_separate_compute_queue();
    selector.add_required_extension(
        VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME);
    auto result = selector.select();

    device.create(result.value());
    if (auto computeResult = device.m_device.get_queue(vkb::QueueType::compute);
        computeResult.has_value()) {
      computeQueue = computeResult.value();
    } else {
      puts("failed to create compute queue");
      exit(-1);
    }
    if (auto res = device.m_device.get_queue_index(vkb::QueueType::compute);
        res.has_value()) {
      computeQueueIndex = res.value();
    } else {
      puts("failed to create compute queue index");
    }

    transferQueue = device.m_device.get_queue(vkb::QueueType::compute).value();
    transferQueueIndex =
        device.m_device.get_queue_index(vkb::QueueType::transfer).value();
    allocator.create(device.m_gpu, device, instance);
    cmdPool.create(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                   computeQueueIndex);
    cmdCompute.alloc(device, cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    computeInput.resize(BUFFER_ELEMENTS);
    computeOutput.resize(BUFFER_ELEMENTS);

    u32 n = 0;
    std::generate(computeInput.begin(), computeInput.end(),
                  [&n] { return n++; });

    bufferSize = BUFFER_ELEMENTS * sizeof(u32);

    hostBuffer = allocator.createBufferExclusive(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    hostBuffer.transferMemory(allocator, (void*)computeInput.data(),
                              hostBuffer.size);
    allocator.flushMapedMemory(hostBuffer);

    deviceBuffer = allocator.createBufferExclusive(
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  }

  void run() {
    cmdCompute.beginWithOneTimeSubmit();
    VkBufferCopy copyRegion{
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = bufferSize,
    };
    cmdCompute.copyBuffer(hostBuffer, deviceBuffer, 1, &copyRegion);
    cmdCompute.end();
    VkSubmitInfo submitInfo{
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext              = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdCompute,
    };

    ezvk::Fence fence;
    fence.create(device, 0);
    vkQueueSubmit(computeQueue, 1, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE,
                    std::numeric_limits<u64>::max());
    cmdCompute.free(device, cmdPool);

    ezvk::DescriptorPoolSizeList lists;
    lists.add(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);

    descriptorPool.create(device, 0, 1, lists.list);

    ezvk::DescriptorSetLayoutBindingList bindingLists;
    bindingLists.add(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                     VK_SHADER_STAGE_COMPUTE_BIT);

    ezvk::DescriptorSetLayout setLayout;
    setLayout.create(device, bindingLists.bindings);

    ezvk::PipelineLayout pipelineLayout;
    pipelineLayout.create(device, {setLayout}, {});

    auto descriptorSet = descriptorPool.allocSet(device, setLayout);

    VkDescriptorBufferInfo   descriptorBI{deviceBuffer, 0, VK_WHOLE_SIZE};
    ezvk::WriteDescriptorSet writeSets;
    writeSets.addBuffer(descriptorSet, 0, 0, 1,
                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &descriptorBI);
    vkUpdateDescriptorSets(device, (u32)writeSets.writeSets.size(),
                           writeSets.writeSets.data(), 0, nullptr);
    ezvk::ComputePipelineBuilder pipelineBuilder;

    struct SpecializationData {
      uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
    } specializationData;

    VkSpecializationMapEntry specializationMapEntry{
        .constantID = 0,
        .offset     = 0,
        .size       = sizeof(u32),
    };

    VkSpecializationInfo specializationInfo{
        .mapEntryCount = 1,
        .pMapEntries   = &specializationMapEntry,
        .dataSize      = sizeof(specializationData),
        .pData         = &specializationData,
    };

    ezvk::Shader compShader;
    if (auto file = ezvk::readFromFile("main.comp.spv", "rb");
        file.has_value()) {
      compShader.create(device, "compute shader", VK_SHADER_STAGE_COMPUTE_BIT,
                        *file, "main", &specializationInfo);
    } else {
      puts("failed to load compute shader");
      exit(-1);
    }

    pipelineBuilder.add(0, compShader, pipelineLayout, nullptr, 0);
    computePipeline = pipelineBuilder.build(device, VK_NULL_HANDLE)[0];

    cmdCompute.alloc(device, cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    ezvk::Fence computeFence;
    computeFence.createSignaled(device);

    cmdCompute.beginWithOneTimeSubmit();

    VkBufferMemoryBarrier barrier{
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_HOST_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = deviceBuffer,
        .size                = VK_WHOLE_SIZE,
    };

    cmdCompute.pipelineBarrier(
        VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_ACCESS_NONE, 0, nullptr, 1, &barrier, 0, nullptr);

    cmdCompute.bindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    cmdCompute.bindDescriptorSetNoDynamic(VK_PIPELINE_BIND_POINT_COMPUTE,
                                          pipelineLayout, 0, 1, &descriptorSet);

    vkCmdDispatch(cmdCompute, BUFFER_ELEMENTS, 1, 1);

    barrier.srcAccessMask       = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.buffer              = deviceBuffer;
    barrier.size                = VK_WHOLE_SIZE;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    cmdCompute.pipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                               VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_NONE,
                               0, nullptr, 1, &barrier, 0, nullptr);
    cmdCompute.copyBuffer(deviceBuffer, hostBuffer, 1, &copyRegion);

    barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask       = VK_ACCESS_HOST_READ_BIT;
    barrier.buffer              = hostBuffer;
    barrier.size                = VK_WHOLE_SIZE;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    cmdCompute.pipelineBufferBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_HOST_BIT, 0, 1,
                                     &barrier);
    cmdCompute.end();

    computeFence.reset(device);
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    VkSubmitInfo computeSubmitInfo{
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext              = nullptr,
        .pWaitDstStageMask  = &waitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdCompute,
    };
    vkQueueSubmit(computeQueue, 1, &computeSubmitInfo, computeFence);
    vkWaitForFences(device, 1, &computeFence, VK_TRUE, 100000000);

    void* mapped;
    allocator.mmap(hostBuffer, &mapped);
    allocator.invalidMappedMemory(hostBuffer);

    memcpy(computeOutput.data(), mapped, bufferSize);
    allocator.munmap(hostBuffer);
    vkQueueWaitIdle(computeQueue);

    puts("compute Input");
    for (auto v : computeInput) {
      printf("%d \t", v);
    }
    putchar('\n');
    puts("compute Output");
    for (auto v : computeOutput) {
      printf("%d \t", v);
    }
    putchar('\n');

    fence.destroy(device);
    computeFence.destroy(device);
    compShader.destroy(device);
    pipelineLayout.destroy(device);
    vkDestroyPipeline(device, computePipeline, nullptr);
    setLayout.destroy(device);
  }
  void destroy() {

    allocator.destroyBuffer(hostBuffer);
    allocator.destroyBuffer(deviceBuffer);

    descriptorPool.destroy(device);
    cmdPool.destroy(device);
    allocator.destroy();
    device.destroy();
    instance.destroy();
  }
};

int main() {
  App app;
  app.init();
  app.run();
  app.destroy();
}
