// question: https://www.zhihu.com/question/620570452
// answer: https://www.zhihu.com/question/620570452/answer/3197758090

#include<iostream>

#include <dsvisual.hpp>

int main(void)
{
    int i,j;

// DSVisual start
    dsvisual::PlatformManager::setRecorder(true);
    dsvisual::ds::Array<int, 10> a;
    a.setVisible(true);
// DSVisual end

    int iTemp;
    puts("Enter number:");
    for(i=0;i<10;i++)
    {
        printf("a[%d]=",i);
        scanf("%d",&a[i]);
    }
    for(i=1;i<10;i++)
    {
        for(j=9;j>=i;j--)
        {
            if(a[j]<a[j-1])
            {
                iTemp=a[j-1];
                a[j-1]=a[j];
                std::getchar();
                a[j]=iTemp;
            }
            std::getchar();
        }
    }
    for(i=0;i<10;i++)
    {
        printf("%d\t",a[i]);
        if(i==4)
        {
            printf("\n");
        }
    }
    return 0;
}