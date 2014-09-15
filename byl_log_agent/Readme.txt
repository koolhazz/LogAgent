1、编译
make clean
make

2、配置
配置在conf目录下，logAgent.conf文件，下面只说说主要要改变的，其他的按时可以不变。
log_file
log文件，最好是绝对路径。

pid_file
pid文件，最好是绝对路径。

unique
表示唯一标识符，最好是机器所在的公网ip。

host
udp绑定的ip。

port
udp绑定的port。

freq
生成文件的频率。

output
文件输出的目录。

3、启动
前台启动
./logAgent -f conf/logAgent.conf

后台启动
./logAgent -f conf/logAgent.conf -D

4、注意点
output目录生成中的文件是以“.”为开头的，生成完毕把“.”去掉。

性能：pf_|^user_plat_auth_cb$|^game_loading$|^newbie_guide$|^feed_loading$|^room_list_loading$|^room_enter_loading$|^room_reloading

事件：by_event
