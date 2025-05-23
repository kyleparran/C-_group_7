#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

static double bd[1001], ak[1001];
static inline int ti(const char* x){return (x[0]-'0')*1000+(x[1]-'0')*100+(x[2]-'0')*10+(x[3]-'0');}
static inline double fs(const char*&p){double v=0,f=0;int s=1,c=0;if(*p=='-'){s=-1;p++;}for(;*p>='0'&&*p<='9';p++)v=v*10+(*p-'0');if(*p=='.'){p++;for(;*p>='0'&&*p<='9';p++){f=f*10+(*p-'0');c++;}}while(c--)f*=0.1;return s*(v+f);}

int main(){
    struct sched_param sp;sp.sched_priority=99;sched_setscheduler(0,SCHED_FIFO,&sp);
    mlockall(MCL_CURRENT|MCL_FUTURE);
    setpriority(PRIO_PROCESS,0,-20);
    cpu_set_t c;CPU_ZERO(&c);CPU_SET(0,&c);sched_setaffinity(0,sizeof(c),&c);

    int u=socket(AF_INET,SOCK_DGRAM,0);int y=1;setsockopt(u,SOL_SOCKET,SO_REUSEADDR,&y,sizeof(y));
    sockaddr_in ui{};ui.sin_family=AF_INET;ui.sin_port=htons(3001);ui.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(u,(sockaddr*)&ui,sizeof(ui));
    ip_mreq mr{};mr.imr_multiaddr.s_addr=inet_addr("239.255.0.1");mr.imr_interface.s_addr=htonl(INADDR_ANY);
    setsockopt(u,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mr,sizeof(mr));
    int big=1<<20;setsockopt(u,SOL_SOCKET,SO_RCVBUF,&big,sizeof(big));

    int t=socket(AF_INET,SOCK_STREAM,0);
    setsockopt(t,IPPROTO_TCP,TCP_NODELAY,&y,sizeof(y));
    sockaddr_in si{};si.sin_family=AF_INET;si.sin_port=htons(4000);
    inet_pton(AF_INET,"127.0.0.1",&si.sin_addr);
    for(int i=0;i<10&&connect(t,(sockaddr*)&si,sizeof(si))<0;i++)usleep(100000);

    char buf[2048];int cid=-1,tg=-1;
    while(true){
        ssize_t n=recv(u,buf,sizeof(buf)-1,0);if(n<=0)continue;buf[n]=0;
        const char* p=buf;const char* e=p+n;
        while(p<e){
            const char* nl=(const char*)memchr(p,'\n',e-p);
            size_t sz=nl?(size_t)(nl-p):(size_t)(e-p);
            if(sz>=4&&!memcmp(p,"SEC|",4)){
                const char* tk=p+4;int ix=ti(tk+3);
                const char* q=p+sz;while(q>p&&*--q!='|');
                const char* r=q;int c=0;while(r>p&&c<2)if(*--r=='|')++c;
                const char* tmp=r+1;double b=fs(tmp);tmp=q+1;double a=fs(tmp);
                bd[ix]=b;ak[ix]=a;
                if(ix==tg&&a!=0){
                    char out[128];
                    int m=snprintf(out,sizeof(out),"CHALLENGE_RESPONSE %d SEC%04d %.10g %.10g Kyle-Parran\n",cid,ix,b,a);
                    send(t,out,m,MSG_NOSIGNAL);
                    tg=-1;
                }
            } else if(sz>=13&&!memcmp(p,"CHALLENGE_ID:",13)){
                cid=atoi(p+13);tg=-1;
            } else if(sz>=7&&!memcmp(p,"TARGET:",7)){
                tg=ti(p+10);
                double b=bd[tg],a=ak[tg];
                if(a!=0){
                    char out[128];
                    int m=snprintf(out,sizeof(out),"CHALLENGE_RESPONSE %d SEC%04d %.10g %.10g Kyle-Parran\n",cid,tg,b,a);
                    send(t,out,m,MSG_NOSIGNAL);
                    tg=-1;
                }
            }
            p=nl?nl+1:e;
        }
    }
}


