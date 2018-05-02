#include "basic.h"

#ifndef CCGL_STRING_FUNCS_H
#define CCGL_STRING_FUNCS_H

namespace ccgl {
/*!
 * \namespace string_funcs
 * \brief String related functions
 */
namespace string_funcs {
/*!
 * \brief Get Uppercase of given string
 * \param[in] str
 * \return Uppercase string
 */
string GetUpper(const string &str);

/*!
 * \brief Match \a char ignore cases
 * \param[in] a, b \a char*
 * \return true or false
 * \sa StringMatch()
 */
bool StringMatch(const char *a, const char *b);

/*!
 * \brief Match Strings in UPPERCASE manner
 * \param[in] text1, text2
 * \return true or false
 */
bool StringMatch(const string &text1, const string &text2);

/*!
 * \brief Trim Both leading and trailing spaces
 * \sa Trim
 * \param[in] str \a string
 */
void TrimSpaces(string &str);

/*!
 * \brief Trim given string's heading and tailing by "<space>,\n,\t,\r"
 * \sa TrimSpaces
 * \param[in] s \a string information
 * \return Trimmed string
 */
string &Trim(string &s);

/*!
 * \brief Splits the given string by spaces
 * \param[in] item \a string information
 * \return The split strings vector
 */
vector<string> SplitString(const string &item);

/*!
 * \brief Splits the given string based on the given delimiter
 * \param[in] item \a string information
 * \param[in] delimiter \a char
 * \return The split strings vector
 */
vector<string> SplitString(const string &item, char delimiter);

/*!
 * \brief Get numeric values by splitting the given string based on the given delimiter
 */
template<typename T>
vector<T> SplitStringForValues(string const &item, char delimiter);

/*!
 * \brief Get int values by splitting the given string based on the given delimiter
 */
vector<int> SplitStringForInt(const string &item, char delimiter);

/*!
 * \brief Get float values by splitting the given string based on the given delimiter
 */
vector<float> SplitStringForFloat(const string &item, char delimiter);

/*!
 * \brief Convert value to string
 * \param[in] val value, e.g., a int, or float
 * \return converted string
 */
template<typename T>
string ValueToString(const T &val);

} /* namespace: string_funcs */
} /* namespace: ccgl */

#endif /* CCGL_STRING_FUNCS_H */
