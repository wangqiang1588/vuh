## Dependencies
- C++14 compliant compiler
- [CMake](https://cmake.org/download/) (build-only)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader)
- [Glslang](https://github.com/KhronosGroup/glslang) (optional, build-only)
- [Catch2](https://github.com/catchorg/Catch2) (optional, build-only)
- [sltbench](https://github.com/ivafanas/sltbench) (optional, build-only)
- [spdlog](https://github.com/gabime/spdlog) (>=1.2.1)

## Compile with dependencies already in place
This assumes required dependencies are already present on your system and findable by ```cmake```.
Replace ```VUH_SOURCE_DIR``` by the path to vuh sources on your setup and build out-of-source.
```bash
cmake ${VUH_SOURCE_DIR}
cmake --build . --target install
```

## Install dependencies & compile
### Linux & Unix-like OSes
Install script depends on ```cmake``` and ```cget``` (```pip install cget```) to be available.
Replace ```VUH_SOURCE_DIR``` and ```DEPENDENCIES_INSTALL_DIR``` by their values on your system.
Build out of source.
```bash
sudo apt-get install build-essential
sudo apt-get install python
sudo apt-get install cmake
sudo apt-get install git
sudo apt-get install libxrandr-dev
sudo apt-get install libspdlog-dev

sudo python -m pip install --upgrade pip
pip install cget --user

export BINPATH=`python -c 'import imp; import os; mod=imp.find_module("cget")[1]; root=os.path.abspath(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(mod))))); print os.path.join(root,"bin")'`
export PATH="$BINPATH:$PATH"

export CGET_PREFIX=${DEPENDENCIES_INSTALL_DIR}
bash ${VUH_SOURCE_DIR}/config/install_dependencies.sh

# glslangValidator
export PATH="${DEPENDENCIES_INSTALL_DIR}/bin/:$PATH"

cmake -DCMAKE_PREFIX_PATH=${DEPENDENCIES_INSTALL_DIR} ${VUH_SOURCE_DIR}
cmake --build . --target install
```
### Windows
TBD.
Should be similar to above.

### macOS
```bash
export PATH="/usr/local/bin:$PATH"
brew install python
brew install python2
brew install glslang
brew install spdlog
brew install doxygen
sudo python -m pip install --upgrade pip
python -m pip install cget
export BINPATH=`python -c 'import imp; import os; mod=imp.find_module("cget")[1]; root=os.path.abspath(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(mod))))); print os.path.join(root,"bin")'`
export PATH="$BINPATH:$PATH"
export CGET_PREFIX=${DEPENDENCIES_INSTALL_DIR}
bash ${VUH_SOURCE_DIR}/config/install_dependencies.sh
export VULKAN_SDK=$(cd "$(dirname ${DEPENDENCIES_INSTALL_DIR})";pwd)
export Catch2_DIR=$(cd "$(dirname ${DEPENDENCIES_INSTALL_DIR})";pwd)
cmake -DCMAKE_PREFIX_PATH=${DEPENDENCIES_INSTALL_DIR} ${VUH_SOURCE_DIR}
cmake --build . --target install
```
macOS do'nt support vulkan ,we need [MoltenVK](https://github.com/KhronosGroup/MoltenVK)
```bash
cd ~
brew install cmake
brew install python
brew install ninja
git clone https://github.com/KhronosGroup/MoltenVK.git
cd MoltenVK
bash fetchDependencies
make
make macos
export VK_ICD_FILENAMES=~/MoltenVK/Package/Release/MoltenVK/macOS/dynamic/MoltenVK_icd.json
```  
run test (build vuh first)
```bash
cd ${VUH_SOURCE_DIR}/test/correctness/
./test_vuh
``` 
