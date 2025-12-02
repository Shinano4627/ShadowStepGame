/* 参考　https://github.com/google/mozc　*/
# pragma once

# include <cassert>
# include <mutex>

class SingletonFinalizer
{
public:
    using FinalizerFunc = void(*)();
    static void addFinalizer(FinalizerFunc func);
    static void finalize(); // シングルトンで作成したクラスを一括開放
};

template <typename T>
class Singleton final 
{
public:
    static T& GetInstance() 
    {
        std::call_once(initFlag, CreateInstanse);   // 一度だけインスタンス生成関数を実行
        assert(s_instance);
        return *s_instance;   // 唯一インスタンス取得
    }

private:
    static void CreateInstanse() 
    {
        s_instance = new T;
        SingletonFinalizer::addFinalizer(&Singleton<T>::DeleteInstanse);
    }

    static void DeleteInstanse() {
        delete s_instance;
        s_instance = nullptr;
    }

    static std::once_flag initFlag;
    static T* s_instance;
};

template <typename T> std::once_flag Singleton<T>::initFlag;
template <typename T> T* Singleton<T>::s_instance = nullptr;
