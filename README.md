# libraig
Client API for R.A.I.G. 

## Build
```
$ cmake .
$ make 
```

## Instructions
- Download the repo
- Extract to your third party library folder
- `cd path/to/libraig-folder`
- Build
- Link against the `libraig` dynamic library in the `lib/` folder. 

To use the API don't forget to include the header file `include/raig.h`, and also connect to `damortonpi.duckdns.org` for testing. 

```
m_RaigAI = new raig::Raig();
m_RaigAI->InitConnection("damortonpi.duckdns.org", "27000");
```


## Dependencies

- libsocket   https://github.com/damorton/libsocket.git

Libraries used in the project are built and installed using cmake. The `libsocket` library will be built into the `external` folder of the libraig directory.

# Use cases

Libsocket is currently being used in the following projects:

- RaigDemo  https://github.com/damorton/raig-demo.git
- Raig3D https://github.com/damorton/RAIG3D.git


