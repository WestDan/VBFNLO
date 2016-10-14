#ifndef H_PARSING_STRING
#define H_PARSING_STRING

#include <locale>
#include <string>
/*
* replace it with isspace

    int is_space(char c)
    {
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' )
	    return 1;
	else 
	    return 0;
    }

// */

/* isspace is supplied bu <locale>, judge space according to locale
 * return value: 0 for non-space, and a specific non-zero value for
 * space char
 */

int find_space(string in, int pos=0)
{
    // find the first space after(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos < size)
    {
	char c = in.at(pos);
	if ( isspace(c) )
	    return pos;
	pos++;
    }
    return -1;
}

int find_pre_space(string in, int pos=0)
{
    // find the first space previor(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos >= 0)
    {
	char c = in.at(pos);
	if ( isspace(c) )
	    return pos;
	pos--;
    }
    return -1;
}

int find_non_space(string in, int pos=0)
{
    // find the first non-space after(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos < size)
    {
	char c = in.at(pos);
	if ( ! isspace(c) )
	    return pos;
	pos++;
    }
    return -1;
}

void parse_string(string in, vector<string> &out, string de = ",")
{
    // remove space around de
    // space are: "\s", "\t", "\n"
    // remove space and 'de' at beginning, if they are
    char c = in.at(0);
    while( isspace(c) || ( c == de ) )
    {
	in.erase(0,1);
	c = in.at(0);
    }

    // find de and remove space before and after it.
    // which means space before and after word is not allowed.

    int pos = 0;
    while ( (pos = in.find(de, pos+1)) != string::npos )
    {
	int pos_1 = find_space(in, pos+1);
	// remove space after de, it relies on that the first char isn't de
	while( (pos_1 - pos) == 1 )
	{
	    in.erase(pos_1, 1);
	    pos_1 = find_space(in,pos+1);
	}
	// remove space before de
	pos_1 = find_pre_space(in, pos-1);
	while( (pos - pos_1) == 1 )
	{
	    in.erase(pos_1, 1);
            pos--;
	    pos_1 = find_pre_space(in,pos-1);
	}
    }

    // but the space in end part of last item is kept.
    int pos_pre = 0;
    while ( (pos = in.find(de, pos_pre)) != string::npos )
    {
	if( pos == pos_pre ) // continuous de
	{
	    pos_pre = pos + 1;
	    continue;
	}
	out.push_back( in.substr( pos_pre, pos - pos_pre) );
	pos_pre = pos+1;
    }

    if( pos_pre < in.length())
    {
	if ( find_non_space(in, pos_pre) >= 0 )
	    out.push_back(in.substr(pos_pre));
    }
}

#endif
