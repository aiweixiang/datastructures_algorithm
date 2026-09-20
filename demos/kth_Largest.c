#include<stdlib.h>
#include<stdio.h>
//找第k大的元素
//法1,冒泡排序
int bubble_sort(int* nums,int numSize,int k)
{
    if(nums==NULL||numSize<=0||k>numSize)
        {   
            printf("arr is invalid or k is invalid\n");
            exit(1);
        }
    for(int i=0;i<numSize-1;++i)
    {
        int swapped=0;
        for(int j=0;j<numSize-1-i;++j)
        {
            if(nums[j]<nums[j+1])
            {
                int temp=nums[j];
                nums[j]=nums[j+1];
                nums[j+1]=temp;
                swapped=1;
            }
        }
        if(!swapped)
            break;
    }
    int ans=nums[k-1];
    return ans;
}
//法2,插入排序
int insert_sort(int*nums,int numSize,int k)
{
    if(nums==NULL||numSize<=0||k>numSize)
        {   
            printf("arr is invalid or k is invalid\n");
            exit(1);
        }
    for(int i=1;i<numSize;++i)
    {
        int key=nums[i];
        int j=i-1;
        while(j>=0&&nums[j]<key)
        //j==-1, 有序区间全部小于key，key排第一
        //nums[j]>=key,key排它后面
        {
            nums[j+1]=nums[j];
            --j;

        }
        nums[j+1]=key;
    }
    return nums[k-1];

}
//维护top-k数组
int find_kth_largest(int*nums, int numSize,int k)
{
    if(nums==NULL||numSize<=0||k>numSize)
        {   
            printf("arr is invalid or k is invalid\n");
            exit(1);
        }
    int* top=malloc(sizeof(int)*k);
    if(top==NULL)
       {
        printf("top arr is invalid\n");
        exit(1);
       } 
    for(int i=0;i<k;++i)
    {
        top[i]=nums[i];
    }
    //insert_sort
    for(int i=1;i<k;++i)
    {
        int x=top[i];
        int j=i-1;
        while(j>=0&&top[j]<x)
        {
            top[j+1]=top[j];
            j--;
        }
        top[j+1]=x;
    }
    //继续读
    for(int m=k;m<numSize;++m)
    {
        int p=nums[m];
        if(p<=top[k-1])
            continue;
        else//p>top[k-1]
        {
            int j=k-2;
            while(j>=0&&top[j]<p)
            {
                top[j+1]=top[j];
                --j;
            }
            top[j+1]=p;
        }
    }
    int ans=top[k-1];
    free(top);
    return ans;
    
}


int main()
{
    printf("输入k值\n");
    int k;
    scanf("%d",&k);
    //冒泡
    int arr[10]={1,4,5,6,9,2,4,6,8,7};
    int brr[10]={1,4,5,6,9,2,4,6,8,7};
    int crr[10]={1,4,5,6,9,2,4,6,8,7};


    
    printf("第%d大的数是%d\n",k,bubble_sort(arr,10,k));
    //插入
    printf("第%d大的数是%d\n",k,insert_sort(brr,10,k));
    //
    printf("第%d大的数是%d\n",k,find_kth_largest(crr,10,k));

    return 0;


}