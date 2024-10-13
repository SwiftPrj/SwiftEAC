# SwiftEAC
An attempt at a semi EAC Emulator

The idea is to hijack the pointer to the EAC driver object and nullify all IO dispatchers. This could technically work by changing the driver a little bit, but I will leave that up to you. This won't work as a fully fledged bypass because of the "heartbeat" which kicks you 5-10 minutes after loading and nullifying the dispatchers in your targetted game. I might work on this in the future to make it working and paste-ready beacuse I find this concept interesting.
