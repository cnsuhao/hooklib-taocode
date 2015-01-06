
template<typename _type>
type_ptr_t<_type>::type_ptr_t( const memory_ptr& ptr )
{

	set_address( ptr.get_address());
}

template<typename _type>
type_ptr_t<_type>::type_ptr_t( const char* module_name,bool load_if_not_exits /*= true */ ):memory_ptr(module_name,load_if_not_exits)
{

}

template<typename _type>
type_ptr_t<_type>::type_ptr_t( mm_address_type _address /*= mm_invalid_address */ ):memory_ptr(_address)
{

}
/*
template<typename _type>
type_ptr_t<_type>::type_ptr_t( size_t _address ):memory_ptr(_address)
{

}*/

template<typename _type>
type_ptr_t<_type>& type_ptr_t<_type>::operator++()
{

	add_self(1);
	return *this;
}

template<typename _type>
type_ptr_t<_type>& type_ptr_t<_type>::operator--()
{
	sub_self(1);
	return *this;
}

template<typename _type>
const type_ptr_t<_type> type_ptr_t<_type>::operator++( int )
{
	type_ptr_t r(*this);
	add_self(1);
	return r;
}

template<typename _type>
const type_ptr_t<_type> type_ptr_t<_type>::operator--( int )
{
	type_ptr_t r(*this);
	sub_self(1);
	return r;
}

template<typename _type>
type_ptr_t<_type> type_ptr_t<_type>::operator+( int type_count ) const
{
	return add( sizeof(_type)*type_count );
}

template<typename _type>
type_ptr_t<_type> type_ptr_t<_type>::operator-( int type_count ) const
{
	return sub( sizeof(_type)*type_count );
}

template<typename _type>
const type_ptr_t<_type>& type_ptr_t<_type>::operator+=( int type_count )
{
	add_self( sizeof(_type)*type_count );
	return *this;
}

template<typename _type>
const type_ptr_t<_type>& type_ptr_t<_type>::operator-=( int type_count )
{
	sub_self( sizeof(_type)*type_count );
	return *this;
}

template<typename _type>
value_visitor_t<_type> type_ptr_t<_type>::operator*() const
{
	return get_address();
}

template<typename _type>
value_visitor_t<_type> type_ptr_t<_type>::operator[]( int index )
{
	return add( sizeof(_type)*index ).get_address();
}

template<typename _type>
value_visitor_t<_type>::value_visitor_t( mm_address_type _address )
{
	set_address(_address);
}

template<typename _type>
void value_visitor_t<_type>::set_address( mm_address_type _address )
{
	value_address = (_type*)_address;
}

template<typename _type>
mm_address_type value_visitor_t<_type>::get_address() const
{
	return (mm_address_type)value_address;
}

template<typename _type>
bool value_visitor_t<_type>::set_value( _type value )
{
	if( !is_readable() && !is_writeable() )return false;

	if(!hook_make_writeable(value_address,sizeof(_type)))return false;

	*value_address = value;
	return true;
}

template<typename _type>
_type value_visitor_t<_type>::get_value() const
{

	if( !is_readable() )return 0;

	return *value_address;
}

template<typename _type>
bool value_visitor_t<_type>::add_value( _type value )
{
	if( !is_readable() && !is_writeable() )return false;

	if(!hook_make_writeable(value_address,sizeof(_type)))return false;

	*value_address += value;
	return true;
}
/*
template<>
bool value_visitor_t<void*>::add_value( void* value )
{
	if( !is_readable() && !is_writeable() )return false;

	if(!hook_make_writeable(value_address,sizeof(void*)))return false;

	*value_address = (void*)((size_t)value_address + (size_t)value);
	return true;
}*/

template<typename _type>
bool value_visitor_t<_type>::sub_value( _type value )
{
	if( !is_readable() && !is_writeable() )return false;

	if(!hook_make_writeable(value_address,sizeof(_type)))return false;

	*value_address -= value;
	return true;
}

template<typename _type>
bool value_visitor_t<_type>::is_readable() const
{
	return 0!=hook_is_readable( value_address,sizeof(_type) );
}

template<typename _type>
bool value_visitor_t<_type>::is_writeable() const
{
	return 0!=hook_is_writeable( value_address,sizeof(_type) );
}

template<typename _type>
bool value_visitor_t<_type>::operator=( _type value )
{

	return set_value(value);
}

template<typename _type>
value_visitor_t<_type>::operator const _type() const
{
	return get_value();
}

template<typename _type>
value_visitor_t<_type>& value_visitor_t<_type>::operator++()
{

	add_value(1);
	return *this;
}

template<typename _type>
value_visitor_t<_type>& value_visitor_t<_type>::operator--()
{
	sub_value(1);
	return *this;
}

template<typename _type>
const value_visitor_t<_type> value_visitor_t<_type>::operator++( int )
{
	value_visitor_t r(*this);

	add_value((_type)1);
	return r;
}

template<typename _type>
const value_visitor_t<_type> value_visitor_t<_type>::operator--( int )
{
	value_visitor_t r(*this);

	sub_value(1);
	return r;
}

template<typename _type>
_type value_visitor_t<_type>::operator+( _type value ) const
{
	return get_value()+value;
}

template<typename _type>
_type value_visitor_t<_type>::operator-( _type value ) const
{
	return get_value()-value;
}

template<typename _type>
const value_visitor_t<_type>& value_visitor_t<_type>::operator+=( _type value )
{

	add_value(value);
	return *this;
}

template<typename _type>
const value_visitor_t<_type>& value_visitor_t<_type>::operator-=( _type value )
{
	sub_value(value);
	return *this;
}
