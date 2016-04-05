# libraig

Client API for R.A.I.G. server application. `libraig` facilitates communication between the game client and the remote RAIG server application by sending path requests and commands while receiving and processing results. 

## Build Instructions Linux
- Download the repo
- Extract to your third party library folder `\external\`
- `cd external\libraig-master`
- Run `build-linux.sh`

Libraries built into `lib\` directory

## Build Instructions Win32
- Download the repo
- Extract to your third party library folder `\external\`
- `cd external\libraig-master`
- Run `build-win32.sh`
- Open `libraig.sln` in Visual Studio
- Build solution

Libraries built into `lib\` directory. The same process applies for other versions of Windows, use the appropriate build script to create the VS files. Run `clean.sh` before each new build.

## Build Instructions Android
- Download the repo
- Extract to your third party library folder `\external\`
- Update your projects `Android.mk` file to include the `*.c` source files
- Build using `NDK`

There are pre-built libraries in the `libs\android` directory 

### Important
- Link against the `libraig` dynamic library in the `lib/` folder. 

To use the API don't forget to include the header file `include/raig/raig_client.h` and link against the dynamic library in the `lib/` directory.

```
m_RaigAI = new raig::Raig();
m_RaigAI->InitConnection("127.0.0.1", "27000");
```

## Dependencies

- CMake   https://cmake.org/
- libsocket

Libraries used in the project are built and installed using cmake. 
