#ifndef LOG_HPP
#define LOG_HPP

template <typename subject_type>
void _log(subject_type subject)
{
    std::cout << subject << std::endl;
}

template <typename first_type, typename... remaining>
void _log(first_type first, remaining... parameters)
{
    std::cout << first;
    _log(parameters...);
}

template <typename... types>
void log(types... parameters)
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "[ " << std::put_time(std::localtime(&now_c), "%T") << " ] ";
    _log(parameters...);
}

#endif /* LOG_HPP */