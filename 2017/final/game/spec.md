## 传输协议
采用基于section的文件格式传输。一个section代表一类信息

> stream

语法规则如下：(section[n] 意味着 section的type字段为n)
<pre>
stream: section[0] section* section[1]
section: type size data crc32
type: int
size: int
data: char*
crc32: int
</pre>

> section:

<pre>
type 0: start section, the size field should be zero.
type 1: end section, the size field should be zero.
type 2: server to client, add object.
type 3: server to client, delete object.
type 4: server to client, change object.
type 5: server to client, clear the screen, the size field should be zero.
type 6: client to server, user key input.
type 7: client to server, user text input.
type 666: client to server, apply the mod.
</pre>

> data:

针对不同section的data格式(epson代表空集; (...)[+n] 表示至少出现n次以上,
xxx yyy(n)代表switch(xxx) {case n: yyy}; [x,x,...]代表向量)
<pre>
type 0: epson
type 1: epson
type 2: (object_id object_type object_raw object_z_index object_pos object_is_show)[+1]
type 3: object_id
type 4: (object_id (change_type object_raw(0) | object_z_index(1) | object_rotate(2) | object_type(3) | object_pos(4) | is_show(5))[+1])[+1]
change_type: int
object_id: int
object_raw: text | bit_image | animate | map | item | menu
object_z_index: int
object_rotate: [float, float, float]
object_type: int
object_father: object_id
object_pos: [x, y]
is_show: bool
type 5: epson
type 6: char
type 7: (char)[+1]
type 666: (hash object_type object_raw)[+1]
</pre>

> object raw

对于不同的object raw，有其不同的gc策略，比如text，其作用域应当是该段对话中，完了之后就该被free。
对于map则不同，其作用域应当是场景切换前一直存在。这里定义下各个object raw:
<p>&符号表示其为计算属性（可变），比如map的bool[m*n]是表示[x,y]的地方是否为空
<pre>
map: w h pixels[w*h] &is_free[w*h]
item: id (entity bitmap(0) text(1)) &script_handle
bit_image: w h has_color ((b_or_w[w*h])[+0](0) (pixels[w,h])[+0](1)) &is_free[w*h]
text: char*
w: int
h: int
has_color: bool
b_or_w: bool
pixels: [R, G, B]
R: int
G: int
B: int
entity: text | bit_image
</pre>


## 物件 场景

物件，就是我们在上面说到的一系列object，比如map，比如item，比如bitmap。。。

场景在这里是物件的集合，比如对于menu来说：
<pre>
menu: (bitmap)[+1] (&item)[+0]
</pre>
我们认为一个menu就是一个场景（scene），至于为什么这里的是item，
因为其存在响应，应当使用item

## 响应

对于一个物件存在对应的响应，同理对一个场景也存在全局响应，比如你在某个地图打开一个宝箱。
然后你设定的是"j"按键是确定，那么，当人物站在物件前按下j的时候，整个响应流程：

j -> network传输 -> scene的handle:
if(人物前的物件) {
    if(物件的handle存在) {
        调用handle
    }
}


## 父子级别：

<pre>
资源集合的单位是scene，一个scene中包含该scene所有资源
但如果需要保持某些特殊的资源，比如说菜单资源，它不该在地图切换的时候变化，
所以scene可以有父子关系，同理各种object都会存在这种类包含关系。
所以可以给每个object都设定一个父级，如果它是最高了，那么父级就是0，否则的话就是父级的object id
当然父子关系是运行时加上去的，对于编写游戏脚本的人不用担心这些
然后一个object的存在周期应当小于等于其father的存在周期
</pre>
例如人物弹出对话，这时scene的位置关系应当是对话框scene在上面，地图scene在下
对话框是一个bitmap(背景) + text物件
这bitmap的father应当是0，然后text物件的father应当是bitmap，这样当bitmap消失的时候，文字也会对应消失

在比如说对于一个地图scene。显然map物件是最高级，然后item物件依附在map上，
显然item的father应当是map，这样map消失的时候item也会消失。

如果要让子物件先于父物件消失，可以通过脚本手动控制

##开发接口

待定。。。