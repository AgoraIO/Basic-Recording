cd src
node-gyp clean
node-gyp configure
node-gyp build
cp build/Release/agorasdk.node ../.