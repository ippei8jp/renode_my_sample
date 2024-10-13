# Renodeで色々試すためのサンプルプログラム

Renodeを色々試すため、なるべくシンプルにプログラムを書いてみた。  

## プログラムのbuild
```
mkdir -p build && cd build
cmake ..
make
cd ..
```


## renodeを起動
```
renode script/renode-config.resc
```

## ブラウザでGUIを表示
ブラウザで``localhost:8000``に接続  


## ターゲットプログラムの起動
Renodeコンソールで以下を実行する  
```
start
```

ブラウザのLEDにGPIOの出力値が反映される。  
ブラウザのスイッチをクリックし、ONにすると割り込みが発生し、割り込みハンドラが起動される。  


