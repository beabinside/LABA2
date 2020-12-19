#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H
#include <optional>
#include <algorithm>
#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
using namespace std;
template <typename T>
class prefix_tree
{
    public:
        // тип функции для метода for_each
        using action_type = function<void(const string &key, const T &value)>;

    private:
        struct node
        {
            using node_ptr = node*;
            // чтобы char не было отрицательным, используем тип node::char_type
            using char_type = unsigned char;

            // следующая нода для каждого символа
            node_ptr next[256];
            // родительская нода
            node_ptr prev = nullptr;
            // хранимое значение, которого может не быть на ноде
            // если нет то value == nullopt
            optional<T> value;
            // количество хранимых значений(не нод) в поддереве
            int count = 0;

            node(node_ptr prev)
                : prev(prev)
            {
                fill(next, next + 256, nullptr);
            }

            ~node()
            {
                for (node_ptr ptr : next)
                {
                    if (ptr)
                    {
                        delete ptr;
                    }
                }
            }
        };
        using node_ptr = typename node::node_ptr;

        // самая первая вершина для сроки ""
        node_ptr root;

        node_ptr find_from_existed(const std::string &key) const
        {
            node_ptr current_node = root;
            // идем по каждому символу в строке
            for (typename node::char_type ch : key)
            {
                assert(current_node);
                node_ptr &next_node = current_node->next[ch];
                if (next_node)
                {
                    // если есть нода для следующего символа, то идет в нее
                    current_node = next_node;
                }
                else
                {
                    // иначе нет ноды для текущего ключа
                    return nullptr;
                }
            }
            // пройдя все символы, мы пришли на ноду для текущего ключа
            return current_node;
        }

        node_ptr find_with_creation(const std::string &key)
        {
            node_ptr current_node = root;
            // прибавляем счетчик ключей для корня
            root->count++;
            // идем по каждому символу в строке
            for (typename node::char_type ch : key)
            {
                assert(current_node);
                node_ptr &next_node = current_node->next[ch];
                if (next_node)
                {
                    // если есть нода для следующего символа, то идет в нее
                    current_node = next_node;
                }
                else
                {
                    // иначе создаем отсутствующую ноду
                    next_node = new node(current_node);
                    current_node = next_node;
                }
                // прибавляем счетчик ключей дляподдерева
                current_node->count++;
            }
            // пройдя все символы, мы пришли на ноду для текущего ключа
            return current_node;
        }

        void remove_node(node_ptr ptr)
        {
            if (ptr && ptr->value) {
                // нода существует и хранит значение
                // удаляем значение
                ptr->value.reset();
                // уменьшаем счетчик хранимых ключей
                ptr->count--;
                assert(ptr->count >= 0);
                // если существует предок
                while (ptr->prev) {
                    // переходим в предка
                    ptr = ptr->prev;
                    // уменьшаем счетчик хранимых ключей
                    ptr->count--;
                    assert(ptr->count >= 0);
                }
            }
        }

        void for_each(node_ptr ptr, string &prefix, action_type action) const
        {
            // если ноды не существует или не хранит ключей, то и обходить нечего
            if (!ptr || !ptr->count) {
                return;
            }
            // если хранится число, то мы вызываем пользовательскую функцию
            if (ptr->value)
            {
                action(prefix, *ptr->value);
            }
            // идем по всем возможным символам
            typename node::char_type ch = 0;
            for (node_ptr next_ptr : ptr->next)
            {
                // если есть не пустая нода
                if (next_ptr && next_ptr->count)
                {
                    // добавляем к префиксу текущий символ, чтобы получить новый префикс для следующей ноды
                    prefix.push_back(ch);
                    // вызываем такой же поиск ключей
                    for_each(next_ptr, prefix, action);
                    // после поиска удаляем добавленный символ
                    prefix.pop_back();
                }
                // следующий символ
                ch++;
            }
        }

        void find_keys(node_ptr ptr, string &prefix, vector<string> &subkeys) const
        {
            // идем по всем парам ключ-значение для prefix и добавляем в вектор ключ
            for_each(ptr, prefix, [&](const string &key, const T &value) {
                subkeys.push_back(key);
            });
        }

        bool equals(node_ptr left, node_ptr right) const
        {
            // если указатели равны, от значит та же самая нода
            if (left == right)
            {
                return true;
            }

            bool left_exist = left && left->count;
            bool right_exist = right && right->count;

            // если одна из нод пустая, а другая нет, то они не равны
            if (left_exist ^ right_exist)
            {
                return false;
            }
            // если обе пустые, то равны
            if (!left_exist && !right_exist) {
                return true;
            }

            // проверка на количество ключей
            if (left->count != right->count)
            {
                return false;
            }
            // проверка на хранимое значение
            if (left->value != right->value)
            {
                return false;
            }
            // проверка дочерних поддеревьев
            for (int i = 0; i < 256; ++i)
            {
                if (!equals(left->next[i], right->next[i]))
                {
                    return false;
                }
            }
            // если все проверки прошли успешно, то значит поддеревья равны
            return true;
        }

    public:
        prefix_tree()
            : root(new node(nullptr))
        {
        }

        ~prefix_tree()
        {
            delete root;
        }

        vector<string> find_keys(const string &prefix) const {
            // ищем ноду
            node_ptr value_node = find_from_existed(prefix);
            // если пустая, то возращаем пустой список
            if (!value_node || !value_node->count) {
                return {};
            }

            vector<string> subkeys;
            string internal_prefix = prefix;
            // вызываем приватный метод поиска
            find_keys(value_node, internal_prefix, subkeys);
            return subkeys;
        }

        bool find(const std::string &key) const
        {
            node_ptr ptr = find_from_existed(key);
            // возвращаем true только если нода не пустая
            return ptr && ptr->value;
        }

        void add(const std::string &key, const T &value)
        {
            // ищем с добавлением нод
            node_ptr value_node = find_with_creation(key);
            assert(value_node);
            assert(!value_node->value);
            // сохраняем значение
            value_node->value.emplace(value);
        }

        void remove(const std::string &key)
        {
            // находим ноду
            node_ptr value_node = find_from_existed(key);
            // удаляем
            remove_node(value_node);
        }

        T& operator[](const std::string &key)
        {
            node_ptr value_node = find_from_existed(key);
            assert(value_node && value_node->value);
            return *value_node->value;
        }

        bool operator==(const prefix_tree<T> &other) const
        {
            return equals(root, other.root);
        }

        bool operator!=(const prefix_tree<T> &other) const
        {
            return !equals(root, other.root);
        }

        int size() const
        {
            return root->count;
        }

        void for_each(action_type action) const
        {
            string prefix;
            for_each(root, prefix, action);
        }

        void to_file(const string &filename) const
        {
            ofstream fout(filename);
            out(fout);
        }
        void from_file(const string &filename) const
        {
            ifstream fin(filename);
            in(fin);
        }

        istream& in(istream &in)
        {
            assert(size() == 0);
            int count;
            in >> count;
            for (int i = 0; i < count; ++i)
            {
                string key;
                T value;
                in >> key >> value;
                add(key, value);
            }
            return in;
        }

        ostream& out(ostream &out) const
        {
            out << size() << endl;
            for_each([&](const string &key, const T &type)
            {
                out << key << endl << type << endl;
            });
            return out;
        }
};


ostream& operator << (ostream &out, const vector<string> &words)
{
    out << "{ ";
    bool before = false;
    for (auto &word : words) {
        out << (before ? ", " : "") << '\"' << word << '\"';
        before = true;
    }
    return out << " }";
}
#endif // PREFIX_TREE_H
