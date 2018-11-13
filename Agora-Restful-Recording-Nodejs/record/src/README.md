# Analysis and Workaround

If we run the `node-gyp rebuild` command manually to build for node.js, we see that the `common.gypi` file used by node-gyp when building for node (7.1.0 on my system) is `~/.node-gyp/7.1.0/include/node/common.gypi
` whereas when we build for electron, we are using `~/.electron-gyp/.node-gyp/iojs-1.4.14/common.gypi`.

## Workaround

We can get this to compile if we add the following fragment to `binding.gyp` in this project:
This should be added as a property of the object in the `targets` array, so that the complete `binding.gyp` becomes:

```
{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ "hello.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "conditions": [
        [ 'OS=="linux"', {
            'library_dirs': [
                './sdk/lib/'
                ],
            "link_settings": {
                "libraries": [],
            },
            'sources': [],
            'defines!': [],
          }
        ]
      ]
    }
  ]
}
```
