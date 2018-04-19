# Node-disk
Node-disk is a C++ addon node.js module for get physical disk information. Now it support windows platform, will support linux & macOS soon.

# Usage
npm install node-disk --save

```js
const disk = require('node-disk');  

const info = disk.getDisk((err, data) => {
  //data is disk information
});

//or invoke like this
disk.getDisk().then((data) => {
  //support promise or async/await
}).catch((err) => {
  //error info
});

//or query by volumes

```
# Data Format
Disk information includes total and free size of disk currently. Plan to implement other info in future.
```js
{
  total: '1042.04 GB',
  free: '781.00 GB',
  volumes: [{
      name: 'C:\\',
      total: '78.74 GB',
      free: '31.18 GB'
    }
  ]
}
```

# Todo
- Support linux and macOS
- Get more info about hardware
- Refector by using nan(Native Abstractions for Node.js)

# Licence & copyright
Copyright (c) 2018 Code2Life
Node-disk is licensed under an MIT license. All rights not explicitly granted in the MIT license are reserved. See the included LICENSE file for more details.
