## 動作環境
GLUTのライブラリが入ったcygwin(バージョン 3.2.0-1)上で動作を確認しています．

## ビルド方法
### tar.gzファイルの場合
j19415.tar.gzがあるディレクトリで，次のコマンドを実行ます．
```
$ gzip -dv j19415.tar.gz
$ tar xvf j19415.tar
$ cd j19415
$ make
$ ./j19415
```
### GitHubからダウンロードしてビルドする場合(たぶん)
```
$ git clone https://github.com/Shibaken28/JJJJJJ.git
$ cd JJJJJJ
$ make
$ ./j19415
```


## ゲーム概要
主人公のJ君を操作してJコインを集めるプラットフォームゲームです．

## 操作方法
矢印キーで左右移動，Zキーでジャンプします．
チュートリアルはゲーム内に用意してあります．

## 更新履歴
- ver.1.0.0
  - リリース
- ver.1.1.0
  - 針の判定を変更
- ver.1.1.1
  - マップ端のマリモが壁を貫通するバグを修正
