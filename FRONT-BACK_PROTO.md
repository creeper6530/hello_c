# Frontend-backend protocol

Only file descriptor that can be closed is the **write** end of Front-to-Back; that signals "please exit now" to the backend


## Message type
| Byte | Meaning | Notes |
| ---- | ------- | ----- |
| 0x00 | NOOP | Does nothing |
| 0x01 | ECHO | Echoes back the entire message |


# Return code of worker
| Code | Meaning |
| ---- | ------- |
| 0 | Success |
| 1 | Read error |
| 2 | Write error |
| 127 | Invalid message |