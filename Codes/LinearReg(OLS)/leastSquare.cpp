#include<stdio.h>
#include<cmath>
#include<stdlib.h>
#include<fstream>
#include<iostream>
#include<string>
#include<ctime>
#include<cstring>
#include<algorithm>
#include<map>
#include <sys/time.h>

#define M 1507
#define N 13



using namespace std;

class Least{
    private:

        string as[M][N];
        float a[M][N];
        //double aa[MM][N];
        char points[2000];
        string labels[M];
        //string labelss[MM];
        double P[M];

    public:

        void ReadTXT();
//        bool GetMatrixInverse(double src[N][N],int n,double des[N][N]);
//        double getA(double arcs[N][N],int n);
//        void  getAStart(double arcs[N][N],int n,double ans[N][N]);
        void getCeta();
        void ReadTEST();
        double * mul(double A[N*N],double B[N*N]);
        void LUP_Descomposition(double A[N*N],double L[N*N],double U[N*N],int P[N]);
        double * LUP_Solve(double L[N*N],double U[N*N],int P[N],double b[N]);
        int getNext(int i, int m, int n);
        int getPre(int i, int m, int n);
        void movedata(double *mtx, int i, int m, int n);
        void transpose(double *mtx, int m, int n);
        double * LUP_solve_inverse(double A[N*N]);




};

void Least::ReadTXT(){
    fstream fin;
    fin.open("30%-conf.data", ios::in);
    if (!fin)
    {
        cout << "can not open the file traindata.txt" << endl;
        exit(1);
    }
    map<int,int> ys;
    for (int num = 0; num <= M-1; num++)
    {
        for (int j = 0; j <= N-1; j++)
        {
            fin >> as[num][j];
            ys[num]=0;
            //fin >> points[num*N + j];
          //  cout << a[num][j] << " ";
        }
        //a[num][0] = 1;
        fin >> P[num];

        //cout << P[num] << endl;
    }
    double miss[N]={0};
    int geshu[N]={0};
    for(int qe=0;qe<N;qe++)
    {
        for(int qi=0;qi<M;qi++)
        {
            if(as[qi][qe]!="?")
            {
                a[qi][qe]=atof(as[qi][qe].c_str());
                miss[qe]+=a[qi][qe];
                geshu[qe]++;
            }

        }
    }
    for(int li=0;li<N;li++)
    {
        for(int lo=0;lo<M;lo++)
        {
            if(as[lo][li]=="?")
                a[lo][li]=(double)miss[li]/geshu[li];
        }
    }


}
//void Least::ReadTEST(){
//    fstream fin;
//    fin.open("testdata.txt",ios::in);
//    for(int num=0;num<=MM-1;num++)
//    {
//
//        for(int j=0;j<=N-1;j++)
//        {
//            fin >> aa[num][j];
//            //fin >> points[num*N + j];
//            cout << aa[num][j] << " ";
//        }
//        fin >> labelss[num];
//        cout << labelss[num] << endl;
//    }
//}
//矩阵乘法
double * Least::mul(double A[N*N],double B[N*N])
{
    double *C=new double[N*N]{};
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            for(int k=0;k<N;k++)
            {
                C[i*N+j] += A[i*N+k]*B[k*N+j];
            }
        }
    }

    //若绝对值小于10^-10,则置为0（这是我自己定的）
    for(int i=0;i<N*N;i++)
    {
        if(abs(C[i])<pow(10,-10))
        {
            C[i]=0;
        }
    }

    return C;
}

//LUP分解
void Least::LUP_Descomposition(double A[N*N],double L[N*N],double U[N*N],int P[N])
{
    int row=0;
    for(int i=0;i<N;i++)
    {
        P[i]=i;
    }
    for(int i=0;i<N-1;i++)
    {
        double p=0.0d;
        for(int j=i;j<N;j++)
        {
            if(abs(A[j*N+i])>p)
            {
                p=abs(A[j*N+i]);
                row=j;
            }
        }
        if(0==p)
        {
            cout<< "矩阵奇异，无法计算逆" <<endl;
            return ;
        }

        //交换P[i]和P[row]
        int tmp=P[i];
        P[i]=P[row];
        P[row]=tmp;

        double tmp2=0.0d;
        for(int j=0;j<N;j++)
        {
            //交换A[i][j]和 A[row][j]
            tmp2=A[i*N+j];
            A[i*N+j]=A[row*N+j];
            A[row*N+j]=tmp2;
        }

        //以下同LU分解
        double u=A[i*N+i],l=0.0d;
        for(int j=i+1;j<N;j++)
        {
            l=A[j*N+i]/u;
            A[j*N+i]=l;
            for(int k=i+1;k<N;k++)
            {
                A[j*N+k]=A[j*N+k]-A[i*N+k]*l;
            }
        }

    }

    //构造L和U
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<=i;j++)
        {
            if(i!=j)
            {
                L[i*N+j]=A[i*N+j];
            }
            else
            {
                L[i*N+j]=1;
            }
        }
        for(int k=i;k<N;k++)
        {
            U[i*N+k]=A[i*N+k];
        }
    }

}

//LUP求解方程
double * Least::LUP_Solve(double L[N*N],double U[N*N],int P[N],double b[N])
{
    double *x=new double[N]();
    double *y=new double[N]();

    //正向替换
    for(int i = 0;i < N;i++)
    {
        y[i] = b[P[i]];
        for(int j = 0;j < i;j++)
        {
            y[i] = y[i] - L[i*N+j]*y[j];
        }
    }
    //反向替换
    for(int i = N-1;i >= 0; i--)
    {
        x[i]=y[i];
        for(int j = N-1;j > i;j--)
        {
            x[i] = x[i] - U[i*N+j]*x[j];
        }
        x[i] /= U[i*N+i];
    }
    return x;
}

/*****************矩阵原地转置BEGIN********************/

/* 后继 */
int Least::getNext(int i, int m, int n)
{
  return (i%n)*m + i/n;
}

/* 前驱 */
int Least::getPre(int i, int m, int n)
{
  return (i%m)*n + i/m;
}

/* 处理以下标i为起点的环 */
void Least::movedata(double *mtx, int i, int m, int n)
{
  double temp = mtx[i]; // 暂存
  int cur = i;    // 当前下标
  int pre = getPre(cur, m, n);
  while(pre != i)
  {
    mtx[cur] = mtx[pre];
    cur = pre;
    pre = getPre(cur, m, n);
  }
  mtx[cur] = temp;
}

/* 转置，即循环处理所有环 */
void Least::transpose(double *mtx, int m, int n)
{
  for(int i=0; i<m*n; ++i)
  {
    int next = getNext(i, m, n);
    while(next > i) // 若存在后继小于i说明重复,就不进行下去了（只有不重复时进入while循环）
      next = getNext(next, m, n);
    if(next == i)  // 处理当前环
      movedata(mtx, i, m, n);
  }
}
/*****************矩阵原地转置END********************/

//LUP求逆(将每列b求出的各列x进行组装)
double * Least:: LUP_solve_inverse(double A[N*N])
{
    //创建矩阵A的副本，注意不能直接用A计算，因为LUP分解算法已将其改变
    double *A_mirror = new double[N*N]();
    double *inv_A=new double[N*N]();//最终的逆矩阵（还需要转置）
    double *inv_A_each=new double[N]();//矩阵逆的各列
    //double *B    =new double[N*N]();
    double *b    =new double[N]();//b阵为B阵的列矩阵分量

    for(int i=0;i<N;i++)
    {
        double *L=new double[N*N]();
        double *U=new double[N*N]();
        int *P=new int[N]();

        //构造单位阵的每一列
        for(int i=0;i<N;i++)
        {
            b[i]=0;
        }
        b[i]=1;

        //每次都需要重新将A复制一份
        for(int i=0;i<N*N;i++)
        {
            A_mirror[i]=A[i];
        }

        LUP_Descomposition(A_mirror,L,U,P);

        inv_A_each=LUP_Solve (L,U,P,b);
        memcpy(inv_A+i*N,inv_A_each,N*sizeof(double));//将各列拼接起来
    }
    transpose(inv_A,N,N);//由于现在根据每列b算出的x按行存储，因此需转置

    return inv_A;
}



void Least::getCeta()
{
    int i,j;
    int u,v;
    int p,q,r;
    int x,y,z;
    int l,h;
    int pp,qq;
    int ok=0;
    double ui=0.7*M;

    int uk=floor(ui);
    if(ui-uk>=0.5)
        uk++;
    map<int,int> yss;
    for(int yk=0;yk<M;yk++)
        yss[yk]=0;
    srand((unsigned)time(NULL));
    int randed[M]={0};
    int mm;//j,num;
    int kv=M-1;
    for(int i = 0;i<M;i++)
    {
        while(randed[mm=rand()%M]);
        randed[mm] = i;
    }
    sort(randed,randed+uk);
    for(int pq=0;pq<uk;pq++){
        //cout<<pq+1<<":"<<randed[pq]<<endl;
        yss[randed[pq]]=1;
    }
    int ww=0;
    int cct=0;
    int so=0;
    double aa[uk][N];
    for(int pu=0;pu<M;pu++)
    {
            if(pu==randed[ww])
            {
               for(int ji=0;ji<N;ji++){
                    aa[ww][ji]=a[pu][ji];
                }
                ww++;
            }
            if(ww==uk)
                break;
    }
   // for(int yt=0;yt<uk;yt++)
    //{
        //cout<<so++<<":";
        //for(int up=0;up<N;up++)
            //cout<<aa[yt][up]<<" ";
        //cout<<endl;
    //}
    double b[N][uk];
    double c[N][N];
    double d[N][N];
    double cc[N*N];
    double *dd = new double[N*N]();
    double e[N][M];
    double ceta[N];
    double sum[M];
    for(int g=0;g<=N-1;g++)
        ceta[g]=0;
    for(i=0;i<=uk;i++)
    {
        sum[i]=0;
        for(j=0;j<=N-1;j++)
        {
            b[j][i]=aa[i][j];
        }
    }
    for(pp=0;pp<=N-1;pp++)
    {
        for(qq=0;qq<=uk-1;qq++)
        {
           e[pp][qq]=0;
        }
    }
    for(u=0;u<=N-1;u++)
    {
        for(v=0;v<=N-1;v++)
        {
            c[u][v]=0;
            d[u][v]=0;
        }
    }
    for(p=0;p<=N-1;p++)
    {
        for(q=0;q<=N-1;q++)
        {
            for(r=0;r<=uk-1;r++)
                c[p][q]+=b[p][r]*aa[r][q];
        }
    }
    for(int sk=0;sk<=N-1;sk++)
    {
        for(int sp=0;sp<=N-1;sp++)
            cc[sk*N+sp]=c[sk][sp];
    }
    //GetMatrixInverse(c,N,d);
    dd=LUP_solve_inverse(cc);
    for(int sk=0;sk<=N-1;sk++)
    {
        for(int sp=0;sp<=N-1;sp++)
            d[sk][sp]=dd[sk*N+sp];
    }
    for(x=0;x<=N-1;x++)
    {
        for(y=0;y<=uk-1;y++)
        {
            for(z=0;z<=N-1;z++)
                e[x][y]+=d[x][z]*b[z][y];
        }
    }
    for(l=0;l<=N-1;l++)
    {
        for(h=0;h<=uk-1;h++)
        {
            ceta[l]+=e[l][h]*P[h];

        }
         //cout<<ceta[l]<<endl;
    }
    double rs[M]={0};
    int we=0;
    double ff=0;
    for(int rr=0;rr<M;rr++)
    {
        if(yss[rr]==0)
        {
            for(int rt=0;rt<N;rt++)
                rs[we]+=ceta[rt]*a[rr][rt];
            rs[we]=fabs(rs[we]-P[rr]);
            we++;
        }
    }
    for(int fd=0;fd<M-uk;fd++)
        ff+=rs[fd]*rs[fd];
    double rmsd=sqrt(ff/(M-uk));
    //cout<<"rmsd="<<rmsd<<endl;
    double Q[M];
    int qt=0;
    double cvrmsd=0;
    for(int hb=0;hb<M;hb++)
    {
        if(yss[hb]==0)
        {
            Q[qt]=P[hb];
            qt++;
        }
    }
    sort(Q,Q+M-uk);
    for(int rv=0;rv<M-uk;rv++)
    {
        //cout<<rv+1<<":"<<Q[rv]<<endl;
        cvrmsd+=Q[rv];
    }
    cvrmsd=(double)cvrmsd/(M-uk);
    cvrmsd=(double)rmsd/cvrmsd;
    double nrmsd=(double)rmsd/(Q[M-uk-1]-Q[0]);
    //cout<<"nrmsd="<<nrmsd<<endl;
    //cout<<"cvrmsd="<<cvrmsd<<endl;
    cout<<rmsd<<" "<<nrmsd<<" "<<cvrmsd<<" ";

}

int main()
{

    struct timeval tpstart,tpend;
    float timeuse;
    gettimeofday(&tpstart,NULL);
    Least le;
    le.ReadTXT();
    le.getCeta();
    gettimeofday(&tpend,NULL);
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
    tpend.tv_usec-tpstart.tv_usec;
    timeuse/=1000000;
    cout<<timeuse<<endl;

    return 0;

}
