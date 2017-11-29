#include"kmeans.h"
int  main()
{
	int flag;
	std::cout << "1.所处理数据集带有标签，且标签从0开始\n2.所处理数据集带有标签，且标签从1开始\n3.所处理数据集不具有标签\n";
	std::cin >> flag;
	if (flag == 3)
	{
		Kmeans x = Kmeans();
		x.K_means();
	}
	else 
	Kmeans x = Kmeans(flag);
	system("pause");
}