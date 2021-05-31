#ifndef SEQUALIZEHIST_H
#define SEQUALIZEHIST_H

#include<typeinfo>
#include<memory>
#include <modules/global.h>

namespace sgis
{

typedef unsigned char       UINT8    ;
typedef unsigned short      UINT16   ;
typedef unsigned int        UINT32   ;

/* 直方图均衡化
 * @param pData 数据指针
 * @param count 数据数量
 * @param equalizeFunction  均衡化函数
 * @param doCleanUp 是否释放equalizeFunction内存
 */
template<typename T>
void equalizeHist(T *pData, size_t count, const T* equalizeFunction)
{
    //进行均衡化
    for(unsigned int i = 0; i < count; ++i)
        pData[i] = equalizeFunction[pData[i]];
}

/* 获取均衡化函数
 * @param   pData   数据指针
 * @param   count   数据数量
 * @return  均衡化函数指针
 */
template<typename T>
std::shared_ptr<T> getEqualizeFunction(const T *pData, size_t count)
{
    //检查类型是否符合要求
    const type_info& DataType = typeid (T);
    if(DataType != typeid(UINT8) && DataType != typeid(UINT16) && DataType != typeid(UINT32))
        return nullptr;

    //确定灰度值最大值
    T Max = T(0) - T(1);

    //统计灰度频率分布。会申请Max*sizeof(T)的内存，目前32和64位灰度无法处理
    unsigned int* Distribution = new unsigned int[Max + 1] {T(0)};
    for(unsigned int i = 0; i < count; ++i)
        Distribution[pData[i]]++;

    //计算灰度概率分布函数，并生成均衡化函数
    double* CDFn = new double[Max + 1] {T(0)};
    std::shared_ptr<T> EqFn(new T[Max + 1] {T(0)}, [](T * pData) {delete []pData;});
    CDFn[0] = double(Distribution[0]) / double(count);
    T* pEqFn = EqFn.get();
    pEqFn[0] = round(CDFn[0] * Max);

    for(unsigned int i = 1; i <= Max; ++i)
    {
        CDFn[i] += CDFn[i - 1] + double(Distribution[i]) / double(count);
        pEqFn[i] = round(CDFn[i] * Max);
    }

    delete [] Distribution;
    delete [] CDFn;
    return EqFn;
}

}//namespace sgif


#endif // SEQUALIZEHIST_H
