# SkipList
使用C++实现的简单跳表

## 使用方法

```
    //key的排序规则
    auto cmp = [](const std::string& lhv,const std::string& rhv){
        return lhv < rhv;
    };

    SkipList<std::string,int,decltype(cmp)> list(cmp);
    list.insert("abs",123);
    list.insert("hello",222);
    list.insert("world",2922);
    for(auto it = list.begin();it != list.end();++it){
        std::cout << it->key << " " << it->value << std::endl;
    }

    list.erase("abs");
    for(auto it = list.begin();it != list.end();++it){
        std::cout << it->key << " " << it->value << std::endl;
    }

    auto it = list.find("hello");
    std::cout << it->value << std::endl;

```
