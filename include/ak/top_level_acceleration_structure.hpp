#pragma once
#include <ak/ak.hpp>

namespace ak
{
	class top_level_acceleration_structure_t
	{
		friend class root;
		
	public:
		top_level_acceleration_structure_t() = default;
		top_level_acceleration_structure_t(top_level_acceleration_structure_t&&) noexcept = default;
		top_level_acceleration_structure_t(const top_level_acceleration_structure_t&) = delete;
		top_level_acceleration_structure_t& operator=(top_level_acceleration_structure_t&&) noexcept = default;
		top_level_acceleration_structure_t& operator=(const top_level_acceleration_structure_t&) = delete;
		~top_level_acceleration_structure_t();

		const auto& config() const { return mCreateInfo; }
		const auto& acceleration_structure_handle() const { return mAccStructure.mHandle; }
		const auto* acceleration_structure_handle_ptr() const { return &mAccStructure.mHandle; }
		const auto& memory_handle() const { return mMemory.get(); }
		const auto* memory_handle_ptr() const { return &mMemory.get(); }
		auto device_address() const { return mDeviceAddress; }

		size_t required_acceleration_structure_size() const { return static_cast<size_t>(mMemoryRequirementsForAccelerationStructure.memoryRequirements.size); }
		size_t required_scratch_buffer_build_size() const { return static_cast<size_t>(mMemoryRequirementsForBuildScratchBuffer.memoryRequirements.size); }
		size_t required_scratch_buffer_update_size() const { return static_cast<size_t>(mMemoryRequirementsForScratchBufferUpdate.memoryRequirements.size); }

		const auto& descriptor_info() const
		{
			mDescriptorInfo = vk::WriteDescriptorSetAccelerationStructureKHR{}
				.setAccelerationStructureCount(1u)
				.setPAccelerationStructures(acceleration_structure_handle_ptr());
			return mDescriptorInfo;
		}

		auto descriptor_type() const			{ return vk::DescriptorType::eAccelerationStructureKHR; } 

		void build(const std::vector<geometry_instance>& aGeometryInstances, std::optional<std::reference_wrapper<buffer_t>> aScratchBuffer = {}, sync aSyncHandler = sync::wait_idle());
		void update(const std::vector<geometry_instance>& aGeometryInstances, std::optional<std::reference_wrapper<buffer_t>> aScratchBuffer = {}, sync aSyncHandler = sync::wait_idle());
		
	private:
		enum struct tlas_action { build, update };
		std::optional<command_buffer> build_or_update(const std::vector<geometry_instance>& aGeometryInstances, std::optional<std::reference_wrapper<buffer_t>> aScratchBuffer, sync aSyncHandler, tlas_action aBuildAction);
		buffer_t& get_and_possibly_create_scratch_buffer();
		
		vk::MemoryRequirements2KHR mMemoryRequirementsForAccelerationStructure;
		vk::MemoryRequirements2KHR mMemoryRequirementsForBuildScratchBuffer;
		vk::MemoryRequirements2KHR mMemoryRequirementsForScratchBufferUpdate;
		vk::MemoryAllocateInfo mMemoryAllocateInfo;
		vk::UniqueDeviceMemory mMemory;

		vk::AccelerationStructureCreateInfoKHR mCreateInfo;
		vk::PhysicalDevice mPhysicalDevice;
		vk::Device mDevice;
		//vk::ResultValueType<vk::UniqueHandle<vk::AccelerationStructureKHR, vk::DispatchLoaderDynamic>>::type mAccStructure;
		ak::handle_wrapper<vk::AccelerationStructureKHR> mAccStructure;
		vk::DispatchLoaderDynamic mDynamicDispatch;
		vk::DeviceAddress mDeviceAddress;

		std::optional<buffer> mScratchBuffer;
		
		mutable vk::WriteDescriptorSetAccelerationStructureKHR mDescriptorInfo;
	};

	using top_level_acceleration_structure = ak::owning_resource<top_level_acceleration_structure_t>;
}
