#pragma once

template <typename Container, typename Func>
std::wstring GenerateDuplicateName(
    const std::wstring &originalName, const Container &container, const Func &nameAccessor)
{
    for (int i = 1;; ++i)
    {
        const std::wstring candidateName = originalName + L" (" + boost::lexical_cast<std::wstring>(i) + L")";

        auto it = std::find_if(
            container.cbegin(), container.cend(),
            [&candidateName, &nameAccessor](const typename Container::value_type &item) {
            return nameAccessor(item) == candidateName;
        });

        if (it == container.cend())
            return candidateName;
    }

    // Unreachable
    return std::wstring();
}