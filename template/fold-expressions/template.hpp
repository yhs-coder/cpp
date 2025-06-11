#ifndef FOLD_EXPRESSIONS_TEMPLATE_HPP
#define FOLD_EXPRESSIONS_TEMPLATE_HPP


// 模板折叠
// 一元折叠表达式
// 对每个参数执行非操作，然后将这些操作&&
//  (!args && ...) 相当于 !a && !b && ...,  ... 表示对每个参数进行前面的操作
template <typename... Args>
bool AllNot(const Args&... args) {
    return (!args && ...);
}


#endif//FOLD_EXPRESSIONS_TEMPLATE_HPP
