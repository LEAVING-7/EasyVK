#include <EasyVK/BufferAllocator.hpp>
#include <EasyVK/Command.hpp>
#include <EasyVK/Descriptor.hpp>
#include <EasyVK/Device.hpp>
#include <EasyVK/Instance.hpp>
#include <EasyVK/PipelineBuilder.hpp>
#include <EasyVK/SyncStructures.hpp>

#include <cstdio>
#include <stdexcept>

u32          BUFFER_ELEMENTS = 1000;
VkDeviceSize bufferSize;
struct App {
  ezvk::Instance instance;
  ezvk::Device   device;

  VkQueue computeQueue;
  u32     computeQueueIndex;

  ezvk::CommandPool   cmdPool;
  ezvk::CommandBuffer cmdCompute;

  std::vector<u32> computeInput;
  std::vector<u32> computeOutput;

  ezvk::BufferAllocator allocator;
  ezvk::AllocatedBuffer deviceBuffer, hostBuffer;

  ezvk::DescriptorPool descriptorPool;

  void init() {
    instance.create({}, {}, "ComputeShaders");
    vkb::PhysicalDeviceSelector selector(instance);
    selector.set_minimum_version(1, 1).require_separate_compute_queue();

    auto result = selector.select();
    device.create(result.value(), {}, {});
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

    cmdPool.create(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                   computeQueueIndex);
    cmdCompute.alloc(device, cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    computeInput.resize(BUFFER_ELEMENTS);
    computeOutput.resize(BUFFER_ELEMENTS);

    u32 n = 0;
    std::generate(computeInput.begin(), computeInput.end(),
                  [&n] { return n++; });

    bufferSize = BUFFER_ELEMENTS * sizeof(u32);

    hostBuffer =
        allocator.createBuffer(computeInput, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VMA_MEMORY_USAGE_CPU_ONLY);
    hostBuffer.transferMemory(allocator, (void*)computeInput.data(),
                              hostBuffer.size);
    allocator.flushMapedMemory(hostBuffer);

    deviceBuffer = allocator.createBufferExclusive(
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);
  }

  void run() {
    cmdCompute.beginWithOneTimeSubmit();
    VkBufferCopy copyRegion{.size = bufferSize};
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

    fence.destroy(device);
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

    
  }
  void destroy() {
    descriptorPool.destroy(device);
    cmdPool.destroy(device);
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
