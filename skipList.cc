#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>


//自定义排序规则
//其实还是使用的 比较数据类型自己内部的 <,所以如果key是用户自定义数据类型的话，需要自己重载 <
template<typename T>
struct Less{
    bool operator() (const T& a,const T& b){
        return a < b;
    }
};

template<typename K,typename V,typename Comp = Less<K>>
class SkipList{
private:
    struct SkipListNode{
        int level; //当前节点的level
        const K key; 
        V value;
        SkipListNode** forward;  //level数组

        SkipListNode()
         :key{0},value{0},level{0},forward{0}{
         }
        SkipListNode(K k,V v,int l,SkipListNode* nxt = nullptr)
         :key{k},value{v},level{l}{
            forward = new SkipListNode* [level + 1];
            for(int i = 0;i <= level;++i){
                forward[i] = nxt;
            }
         }
        ~SkipListNode(){
            delete [] forward;
        }
    };

    using node = SkipListNode;

    //初始化函数
    void init(){
        srand((uint32_t)time(NULL));
        level_ = length_ = 0;
        head_->forward = new node* [MAXL + 1];
        for(int i = 0;i <= MAXL;++i){
            head_->forward[i] = tail_;
        }
    }

    //为计算计算出level
    int randomLevel(){
        int lv = 1;
        while((rand() & S) < PS){
            lv++;
        }
        return MAXL > lv ? lv : MAXL;
    }

    int level_;        //节点的最大level
    int length_;       //节点个数

    static const int MAXL = 32;        //最大level
    //下面的成员是计算节点level的时候需要用到的
    static const int P = 4;            
    static const int S = 0xFFFF;
    static const int PS = S / P;
    //static const int INVALID = INT_MAX;


    node* head_;
    node* tail_;  //不存在的标志

    Comp less;  //key 比较仿函数

    node* find(const K& key,node** update){
        node* p = head_;
        for(int i = level_;i >= 0;--i){
            //从head节点的最大level开始查找
            //比较的是下一个节点的key，所以这里在查询的过程中存储的是对于位置的前面信息
            while(p->forward[i] != tail_ && less(p->forward[i]->key,key)){
                p = p->forward[i];
            }
            update[i] = p;
        }
        p = p->forward[0];
        return p;
    }
public:
    //封装的迭代器
    struct Iter{
        node* p;

        Iter():p(nullptr){}
        Iter(node* rhv) : p(rhv){}

        node* operator-> ()const { return (p); }
        node& operator*()const { return *p; }
        bool operator == (const Iter& rhv) { return p == rhv.p; }
        bool operator != (const Iter& rhv) { return !(*this == rhv); }
        void operator++ () { p = p->forward[0]; }
        void operator++ (int) { p = p->forward[0]; }
    };

    SkipList()
     :head_(new node()),tail_(new node()),less{Comp()}{
        init();
    }

    SkipList(Comp _less)
     :head_(new node()),tail_(new node()),less{_less}{
        init();
    }

    Iter begin(){
        return Iter(head_->forward[0]);
    }
    Iter end(){
        return Iter(tail_);
    }

    bool count(const K& key){
        node* update[MAXL + 1];
        node* p = find(key,update);
        if(p == tail_){
            return false;
        }
        return key == p->key;
    }

    Iter find(const K& key){
        node* update[MAXL + 1];
        node* p = find(key,update);
        if(p == tail_){
            return tail_;
        }
        if(p->key != key){
            return tail_;
        }
        return Iter(p);
    }

    void insert(const K& key,const V& value){
        node* update[MAXL + 1];
        node* p = find(key,update);

        if(p->key == key){
            //如果存在拥有相同key的节点，就更新值，然后返回
            p->value = value;
            return;
        }

        //如果节点不存在
        int lv = randomLevel();
        if(lv > level_){
            lv = ++level_;
            update[lv] = head_;
        }
        node* newNode = new node(key,value,lv);
        for(int i = lv;i >= 0;--i){
            p = update[i];
            newNode->forward[i] = p->forward[i];
            p->forward[i] = newNode;
        }
        ++length_;
    }

    bool erase(const K& key){
        node* update[MAXL + 1];
        node* p = find(key,update);
        if(p->key != key){
            return false;
        }
        for(int i = 0;i <= p->level;++i){
            update[i]->forward[i] = p->forward[i];
        }
        delete p;

        while(level_ > 0 && head_->forward[level_] == tail_){
            --level_;
        }
        --length_;
        return true;
    }

};

int main(int argc,char** argv){
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

}

