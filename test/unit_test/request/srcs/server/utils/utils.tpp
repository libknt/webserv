template <typename T>
std::string Utils::toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
