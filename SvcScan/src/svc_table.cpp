/*
*  svc_table.cpp
*  -------------
*  Source file for a network application service table
*/
#include "includes/containers/svc_table.h"

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable()
{
    if (m_list.empty())
    {
        m_list.add(value_type{ "PORT", "STATE", "SERVICE", "INFO", true });
    }
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const SvcTable &t_table) : this_t()
{
    m_list = t_table.m_list;
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const string &t_addr, const vector<value_type> &t_vect)
    : this_t() {

    m_addr = t_addr;

    add(t_vect);
    sort();
}

/**
* @brief  Add a new record to the underlying list of service information.
*/
void scan::SvcTable::add(const value_type &t_info)
{
    m_list.add(t_info);
}

/**
* @brief  Add new records to the underlying list of service information.
*/
void scan::SvcTable::add(const vector<value_type> &t_vect)
{
    m_list.add_range(t_vect);
}

/**
* @brief  Sort the underlying service list by port number (excluding header record).
*/
void scan::SvcTable::sort()
{
    vector<value_type> &vect{ m_list.vector() };
    ranges::sort(vect.begin() + 1, vect.end(), ranges::less(), &value_type::port);
}

/**
* @brief  Determine whether the underlying service information list is empty.
*         Optionally includes the table header element.
*/
bool scan::SvcTable::empty(const bool &t_inc_header) const noexcept
{
    return size(t_inc_header) == 0;
}

/**
* @brief  Get the size of the underlying service information list. Optionally
*         includes the table header element.
*/
size_t scan::SvcTable::size(const bool &t_inc_header) const noexcept
{
    return t_inc_header || m_list.empty() ? m_list.size() : m_list.size() - 1;
}

/**
* @brief  Get a constant iterator to the first element in the underlying list.
*/
scan::SvcTable::iterator scan::SvcTable::begin() const noexcept
{
    return m_list.begin();
}

/**
* @brief  Get a constant iterator to the past-the-end element in
*         the underlying list.
*/
scan::SvcTable::iterator scan::SvcTable::end() const noexcept
{
    return m_list.end();
}

/**
* @brief  Get a constant reference to the underlying target hostname or IPv4 address.
*/
const std::string &scan::SvcTable::addr() const noexcept
{
    return m_addr;
}

/**
* @brief  Get the underlying HTTP responses as a combined string.
*/
std::string scan::SvcTable::curl_str(const bool &t_colorize) const
{
    sstream stream;
    const List<value_type> info_list{ data() };

    for (const value_type &info : info_list)
    {
        // Ignore invalid responses
        if (!info.response.valid())
        {
            continue;
        }
        string raw_resp{ info.response.str() };

        // Remove the trailing CRLF
        if (raw_resp.ends_with(&CRLF[0]))
        {
            raw_resp = algo::upto_last_eol(raw_resp);
        }

        string start_ln_lbl{ algo::fstr("Port %", info.port()) };
        const string start_ln{ algo::fstr(" : %", info.request.start_line()) };

        const size_t ln_size{ start_ln_lbl.size() + start_ln.size() };

        if (t_colorize)
        {
            start_ln_lbl = stdu::colorize(start_ln_lbl, Color::green);
        }

        const string title = algo::concat(start_ln_lbl,
                                          start_ln,
                                          &LF[0],
                                          algo::underline(ln_size));

        stream << algo::concat(title, &LF[0], raw_resp, &LF[0]);

        if (&info != &info_list.last())
        {
            stream << &LF[0];
        }
    }
    return stream.str();
}

/**
* @brief  Get the underlying service information as a string.
*         Optionally includes the underlying HTTP responses.
*/
std::string scan::SvcTable::str(const bool &t_colorize, const bool &t_inc_curl) const
{
    sstream stream;
    stream << table_str(t_colorize);

    // Include CURL results
    if (t_inc_curl && !empty())
    {
        const bool no_curl = ranges::all_of(data(), [](const value_type &l_info)
        {
            return !l_info.response.valid();
        });

        if (!no_curl)
        {
            stream << algo::concat(&LF[0], curl_str(t_colorize));
        }
    }
    return stream.str();
}

/**
* @brief  Get the underlying service information table as a string.
*/
std::string scan::SvcTable::table_str(const bool &t_colorize) const
{
    sstream stream;

    // Add scan table title
    if (!m_addr.empty())
    {
        stream << table_title(t_colorize);
    }
    const List<value_type> info_list{ data() };

    value_type::no_summary = ranges::all_of(info_list, [](const value_type &l_info)
    {
        return l_info.summary.empty();
    });

    const field_map width_map
    {
        { field_t::port,    max_width(field_t::port) },
        { field_t::state,   max_width(field_t::state) },
        { field_t::service, max_width(field_t::service) },
        { field_t::summary, max_width(field_t::summary) }
    };

    const string sep{ value_type::no_summary ? "    " : "   " };
    string header{ algo::join(begin()->pad_fields(width_map), sep) };

    // Hide the summary field header
    if (value_type::no_summary)
    {
        header = header.substr(0, header.find("SERVICE") + 7);
    }

    header = t_colorize ? stdu::colorize(header, Color::green) : header;
    stream << algo::concat(header, &LF[0]);

    // Pad service fields and add write record to stream
    for (const value_type &info : info_list)
    {
        stream << algo::join(info.pad_fields(width_map), sep) << &LF[0];
    }
    return stream.str();
}

/**
* @brief  Get a copy of the underlying service information list
*         without the field headers included.
*/
scan::List<scan::SvcInfo> scan::SvcTable::data() const
{
    return m_list.slice(1);
}

/**
* @brief  Get the max character width of the given service record field.
*/
size_t scan::SvcTable::max_width(const field_t &t_field) const
{
    size_t max_width{ 0 };

    for (const value_type &info : m_list)
    {
        const size_t width{ info[t_field].size() };
        max_width = width > max_width ? width : max_width;
    }
    return max_width;
}

/**
* @brief  Get the table title using the underlying scan target.
*/
std::string scan::SvcTable::table_title(const bool &t_colorize) const
{
    string title_lbl{ "Target" };
    const string target{ algo::fstr(" : %", m_addr) };

    const size_t ln_size{ title_lbl.size() + target.size() };

    // Colorize the table title
    if (t_colorize)
    {
        title_lbl = stdu::colorize(title_lbl, Color::green);
    }
    const string title{ algo::concat(title_lbl, target) };

    return algo::concat(title, &LF[0], algo::underline(ln_size, '='), &LF[0]);
}
