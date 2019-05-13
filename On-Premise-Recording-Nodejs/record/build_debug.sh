cd src
node-gyp clean
node-gyp configure --debug
node-gyp build
cp build/Debug/agorasdk.node ../.