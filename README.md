1.启动tracker
python bttrack.py --port 6969 --dfile dstate
2.制作种子
3.发布（批量）
python btlaunchmany.py --data_dir /storage/p2pServer/p2pdir/store/ --torrent_dir /storage/p2pServer/p2pdir/store/
4.编译并执行
cd src && make
./p2pServer &
