### libro 非对称协程库
---
主要用途
* 适用于网络I/O密集型业务，典型的有：
  * 广告系统中AdExchange平台会对接多家DSP，媒体发送的一次广告请求到达AdExchange后，AdExchange会将广告请求透传到N个DSP，并将各个DSP的广告返回进行竞价。向N个DSP的广告的请求/返回这一过程可以使用协程来完成。
  * DSP系统一般由DMP、索引服务、算法服务等组成，一般是由AdServer（Mixer）服务器接收到AdExchange发送来的广告请求，先请求DMP拿到用户的profile，再根据用户profile请求索引服务得到广告候选集，再请求算法服务得到排序后最优的广告，最后将广告返回到AdExchange
* 编程方法：以DSP为例，将请求下游各个服务的过程抽象为一个单一的任务，以同步阻塞的方式编写代码:
  * 代码形式为 {send_to(DMP); recv_from(DMP); send_to(索引服务); recv_from(索引服务); send_to(算法服务); recv_from(算法服务); }
  * 协程库负责任务的挂起、切出、重入

协程调度算法
* 调度过程 -> 协程A -> 调度过程 -> 协程B -> 调度过程 -> …

多线程
* 暂不支持worksteal
