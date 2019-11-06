#pragma once

#include "vuh/core/core.hpp"
#include "vuh/mem/allocDevice.hpp"
#include "vuh/mem/basicMemory.hpp"
#include <vuh/device.h>
#include <cassert>

namespace vuh {
    namespace img {

/// Covers basic array functionality. Wraps the Image.
/// Keeps the data, handles initialization, copy/move, common interface,
/// binding memory to buffer objects, etc...
        template<class T, class Alloc>
        class BasicImage: virtual public vuh::mem::BasicMemory, public vhn::Image {
        public:
            static constexpr auto descriptor_class = basic_memory_image_clz;

            /// Construct Image of given size in device memory
            BasicImage(const vuh::Device& dev                     ///< device to allocate array
                    , const vhn::ImageType imageType
                    , const vhn::ImageViewType imageViewType
                    , const size_t width    ///< desired width
                    , const size_t height    ///< desired height
                    , const vhn::Format fmt=vhn::Format::eR8G8B8A8Unorm/// format
                    , const vhn::MemoryPropertyFlags props={} ///< additional memory property flags. These are 'added' to flags defind by allocator.
                    , const vhn::ImageUsageFlags usage={}         ///< additional usage flagsws. These are 'added' to flags defined by allocator.
            )
                    : vhn::Image(Alloc::makeImage(dev, imageType, width, height, fmt, usage, _res))
                    , _dev(dev) {
                VULKAN_HPP_ASSERT(vhn::Result::eSuccess == _res);
                do {
                    if (vhn::Result::eSuccess != _res)
                        break;

                    auto alloc = Alloc();
                    _mem = alloc.allocMemory(_dev, *this, props, _res);
                    if (vhn::Result::eSuccess != _res)
                        break;
                    _flags = alloc.memoryProperties(_dev, *this, props);
                    _dev.bindImageMemory(*this, _mem, 0);
                    _imageView = alloc.makeImageView(_dev, *this, imageViewType, fmt, _res);
                } while(0);
                if (vhn::Result::eSuccess != _res) { // destroy buffer if memory allocation was not successful
                    release();
                #ifndef VULKAN_HPP_NO_EXCEPTIONS
                    throw;
                #endif
                }
            }

            /// Release resources associated with current object.
            ~BasicImage() noexcept {release();}

            BasicImage(const BasicImage&) = delete;
            BasicImage& operator= (const BasicImage&) = delete;

            /// Move constructor. Passes the underlying buffer ownership.
            BasicImage(BasicImage&& other) noexcept
                    : vhn::Image(other), _mem(other._mem), _flags(other._flags), _dev(other._dev) {
                static_cast<vhn::Image&>(other) = nullptr;
            }

            /// @return underlying image
            auto image()-> vhn::Image { return *this; }

            auto imageView() -> vhn::ImageView { return _imageView; }

            auto imageLayout() -> vhn::ImageLayout { return _imageLayout; }

            auto sampler() -> vhn::Sampler { return _sampler; }

            /// @return offset of the current buffer from the beginning of associated device memory.
            /// For arrays managing their own memory this is always 0.
            auto offset() const-> std::size_t { return 0;}

            /// @return reference to device on which underlying buffer is allocated
            auto device()-> vuh::Device& { return _dev; }

            /// @return true if array is host-visible, ie can expose its data via a normal host pointer.
            auto isHostVisible() const-> bool {
                return bool(_flags & vhn::MemoryPropertyFlagBits::eHostVisible);
            }

            /// Move assignment.
            /// Resources associated with current array are released immidiately (and not when moved from
            /// object goes out of scope).
            auto operator= (BasicImage&& other) noexcept-> BasicImage& {
                release();
                _mem = other._mem;
                _flags = other._flags;
                _dev = other._dev;
                reinterpret_cast<vhn::Image&>(*this) = reinterpret_cast<vhn::Image&>(other);
                reinterpret_cast<vhn::Image&>(other) = nullptr;
                return *this;
            }

            /// swap the guts of two basic arrays
            auto swap(BasicImage& other) noexcept-> void {
                using std::swap;
                swap(static_cast<vhn::Image&>(&this), static_cast<vhn::Image&>(other));
                swap(_mem, other._mem);
                swap(_flags, other._flags);
                swap(_dev, other._dev);
            }

            virtual auto descriptorImageInfo() -> vhn::DescriptorImageInfo& override {
                _descImageInfo.setSampler(sampler());
                _descImageInfo.setImageView(imageView());
                _descImageInfo.setImageLayout(imageLayout());
                return _descImageInfo;
            }

            virtual auto descriptorType() const -> vhn::DescriptorType override  {
                return descriptor_class;
            }

        private: // helpers
            /// release resources associated with current BasicArray object
            auto release() noexcept-> void {
                if(static_cast<vhn::Image&>(*this)){
                    if(bool(_mem)) {
                        _dev.freeMemory(_mem);
                    }
                    _dev.destroyImage(*this);
                }
            }
        protected: // data
            vhn::DeviceMemory          _mem;      ///< associated chunk of device memory
            vhn::MemoryPropertyFlags   _flags;    ///< actual flags of allocated memory (may differ from those requested)
            const vuh::Device&         _dev;      ///< referes underlying logical device
            vhn::ImageView             _imageView;
            vhn::Sampler               _sampler;
            vhn::ImageLayout           _imageLayout;
        }; // class BasicImage

        template<class T, class Alloc>
        class Basic2DImage: public BasicImage<T, Alloc> {
            using Base = BasicImage<T, Alloc>;
        public:
            /// Construct Image of given size in device memory
            Basic2DImage(const vuh::Device& dev                     ///< device to allocate array
                    , const size_t width                     ///< desired width in bytes
                    , const size_t height                     ///< desired height in bytes
                    , const vhn::Format fmt=vhn::Format::eR8G8B8A8Unorm /// format
                    , const vhn::MemoryPropertyFlags props={} ///< additional memory property flags. These are 'added' to flags defind by allocator.
                    , const vhn::ImageUsageFlags usage={}         ///< additional usage flagsws. These are 'added' to flags defined by allocator.
            )
                    : Base(dev, vhn::ImageType::e2D, vhn::ImageViewType::e2D, width, height, fmt, props, usage)
                    , _width(width)
                    , _height(height)
            {}

            /// @return size of a memory chunk occupied by array elements
            /// (not the size of actually allocated chunk, which may be a bit bigger).
            auto size_bytes() const-> uint32_t {return _height * _width * sizeof(T);}

            /// @return number of elements
            auto size() const-> size_t {return _height * _width;}

            auto width() const-> uint32_t {return _width;}

            auto height() const-> uint32_t {return _height;}

        private: // helpers
            size_t                                      _width;    ///< desired width in bytes
            size_t                                      _height;   ///< desired height in bytes
        }; //// class Basic2DImage
    } // namespace img
} // namespace vuh