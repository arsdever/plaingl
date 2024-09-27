#include <GLFW/glfw3.h>

#include "tools/console/console.hpp"

#include "common/logging.hpp"
#include "common/utils.hpp"
#include "core/command_dispatcher.hpp"
#include "core/input_system.hpp"

namespace
{
logger log() { return get_logger("console"); }
} // namespace

struct console::impl
{
    core::command_dispatcher dispatcher;
    std::string input;
    bool is_active { false };
    std::array<std::string, 100> history;
    size_t history_counter { 0 };
    size_t history_size { 0 };
    struct command_lookup_node
    {
        std::string key;
        std::string path;
        std::function<std::shared_ptr<core::command_base>(
            const std::vector<std::string_view>&)>
            func;
        std::unordered_map<std::string,
                           command_lookup_node,
                           string_hash,
                           std::equal_to<>>
            children;
    };

    std::unordered_map<std::string,
                       command_lookup_node,
                       string_hash,
                       std::equal_to<>>
        _lookup_tree;

    void eval()
    {
        if (input.empty())
            return;

        std::string str = input;
        input.clear();
        auto tokens = console::tokenize(str);
        if (tokens.empty())
            return;

        auto token_it = tokens.begin();
        std::string path;
        command_lookup_node* node = nullptr;
        path = std::string(*token_it);
        if (auto it = _lookup_tree.find(*(token_it++));
            it != _lookup_tree.end())
        {
            node = &it->second;
        }

        while (token_it != tokens.end() && node != nullptr)
        {
            auto it = node->children.find(*token_it);
            if (it == node->children.end())
            {
                break;
            }
            node = &it->second;
            path += "." + std::string(*token_it);
            ++token_it;
        }

        if (node == nullptr)
        {
            log()->warn("No command was registered for \"{}\"", path);
            return;
        }

        if (node->func == nullptr)
        {
            log()->warn("Invalid command path \"{}\"", path);
            return;
        }

        auto args = std::vector<std::string_view>(token_it, tokens.end());
        dispatcher.dispatch(node->func(args));
        log()->info(str);
    }
};

console::console()
    : _impl(std::make_unique<impl>())
{
}

console::~console() { }

void console::input(int ch)
{
    if (ch == GLFW_KEY_ENTER || ch == GLFW_KEY_KP_ENTER)
        _impl->eval();
    else
    {
        if (ch == GLFW_KEY_BACKSPACE)
        {
            if (_impl->input.size() > 0)
                _impl->input.pop_back();
        }
        else if (ch < 256)
        {
            auto mods = core::input_system::get_modifiers();
            bool upper = (mods & core::input_system::modifiers::Shift) ^
                         (mods & core::input_system::modifiers::CapsLock);
            if (isalpha(ch))
            {
                ch = upper ? std::toupper(ch) : std::tolower(ch);
            }
            else if (upper)
            {
                static const std::unordered_map<int, int> symbol_alter {
                    { '-', '_' },  { '=', '+' }, { '[', '{' },  { ']', '}' },
                    { '\\', '|' }, { ';', ':' }, { '\'', '"' }, { '`', '~' },
                    { ',', '<' },  { '.', '>' }, { '/', '?' },  { ' ', ' ' },
                    { '1', '!' },  { '2', '@' }, { '3', '#' },  { '4', '$' },
                    { '5', '%' },  { '6', '^' }, { '7', '&' },  { '8', '*' },
                    { '9', '(' },  { '0', ')' },
                };

                if (auto it = symbol_alter.find(ch); it != symbol_alter.end())
                {
                    ch = it->second;
                }
            }
            _impl->input += ch;
        }
    }

    log()->trace("Input: \"{}\"", _impl->input);
}

void console::input(std::string in)
{
    auto lines = tokenize(in, [](int c) -> int { return c == '\n'; });

    for (auto& line : lines)
    {
        _impl->input = line;
        _impl->eval();
    }
}

bool console::is_active() const { return _impl->is_active; }

void console::activate() { _impl->is_active = true; }

void console::deactivate() { _impl->is_active = false; }

void console::enable_autoactivation()
{
    core::input_system::on_keypress += [ this ](int keycode)
    {
        if (keycode == GLFW_KEY_GRAVE_ACCENT)
        {
            if (is_active())
            {
                deactivate();
                log()->debug("Console deactivated");
            }
            else
            {
                activate();
                log()->debug("Console activated");
            }
        }
        else if (is_active())
        {
            input(keycode);
        }
    };
}

core::command_dispatcher& console::processor() { return _impl->dispatcher; }

std::vector<std::string_view> console::tokenize(std::string_view in,
                                                int (*delim)(int))
{
    std::vector<std::string_view> result = {};

    auto window_begin = in.begin();
    auto window_end = in.begin();
    while (window_begin != in.end())
    {
        while (window_begin != in.end() && delim(*window_begin))
        {
            window_begin++;
        }

        window_end = window_begin;

        while (window_end != in.end() && !delim(*window_end))
        {
            window_end++;
        }

        if (window_begin != window_end)
            result.push_back(std::string_view(window_begin, window_end));

        window_begin = window_end;
    }

    return result;
}

std::string_view console::get_current_input() const { return _impl->input; }

void console::register_command(
    std::string_view command_path,
    std::function<std::shared_ptr<core::command_base>(
        const std::vector<std::string_view>&)> func)
{
    auto tokens = tokenize(command_path, [](int c) -> int { return c == '.'; });

    auto token_it = tokens.begin();
    auto path = std::string(*token_it);
    impl::command_lookup_node* node = nullptr;
    node = &_impl->_lookup_tree[ std::string(*token_it) ];
    node->key = *token_it;
    node->path = path;
    ++token_it;

    while (token_it != tokens.end())
    {
        path += "." + std::string(*token_it);
        node = &node->children[ std::string(*token_it) ];
        node->key = *token_it;
        node->path = path;
        ++token_it;
    }

    node->func = func;
}

void console::register_for_logs()
{
    class console_sink
        : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
    {
    public:
        console_sink(console* c)
            : _console(c)
        {
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            _console->add_history(
                std::string(msg.payload.data(), msg.payload.size()));
        }
        void flush_() override { }

    private:
        console* _console;
    };

    auto sink = std::make_shared<console_sink>(this);
    std::static_pointer_cast<spdlog::sinks::dist_sink_st>(
        spdlog::default_logger()->sinks()[ 0 ])
        ->add_sink(sink);
    sink->set_level(spdlog::level::info);
}

size_t console::history_size() const { return _impl->history_size; }

void console::add_history(std::string_view text)
{
    ++_impl->history_size;
    _impl->history[ _impl->history_counter++ % _impl->history.size() ] =
        std::move(text);
}

std::string_view console::history_element(int index) const
{
    return _impl->history[ (_impl->history_counter - index - 1) %
                           _impl->history.size() ];
}

template <>
std::string console::convert<std::string>(std::string_view in)
{
    return std::string(in);
}

template <>
int console::convert<int>(std::string_view in)
{
    return std::stoi(std::string(in));
}

template <>
long console::convert<long>(std::string_view in)
{
    return std::stol(std::string(in));
}

template <>
long long console::convert<long long>(std::string_view in)
{
    return std::stoll(std::string(in));
}

template <>
unsigned long console::convert<unsigned long>(std::string_view in)
{
    return std::stoul(std::string(in));
}

template <>
unsigned long long console::convert<unsigned long long>(std::string_view in)
{
    return std::stoull(std::string(in));
}

template <>
float console::convert<float>(std::string_view in)
{
    return std::stof(std::string(in));
}

template <>
double console::convert<double>(std::string_view in)
{
    return std::stod(std::string(in));
}
