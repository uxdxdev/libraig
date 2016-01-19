# libraig
Client API for R.A.I.G. server application. `libraig` facilitates communication between the game client and the remote RAIG server application by sending path requests and commands while receiving and processing results. 

## Instructions
- Download the repo
- Extract to your third party library folder
- `cd libraig-master/`
- Build
```
$ cmake .
$ make 
```
- Link against the `libraig` dynamic library in the `lib/` folder. 

To use the API don't forget to include the header file `include/raig/raig.h` and link against the dynamic library in the `lib/` directory.

```
m_RaigAI = new raig::Raig();
m_RaigAI->InitConnection("127.0.0.1", "27000");
```

## Dependencies

- libsocket   https://github.com/damorton/libsocket.git

Libraries used in the project are built and installed using cmake. The `libsocket` library will be built into the `external` folder of the libraig directory.

# Use cases

Libraig is currently being used in the following projects:

- Raig3D https://github.com/damorton/RAIG3D.git
- RaigDemo  https://github.com/damorton/raig-demo.git


