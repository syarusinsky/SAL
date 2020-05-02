#ifndef VARIANT_HPP
#define VARIANT_HPP

/*************************************************************************
 * The crudest 'variant' type possible. It stores a raw pointer and
 * provides a templated get() function to return the data in the
 * correct type. Of course it is up to the user to request the correct
 * type from the template.
*************************************************************************/

class Variant
{
	public:
		Variant (void* data) : m_Data (data) {}
		~Variant() {}

		template <typename T>
		T get() const { return *(static_cast<T*>(m_Data)); };
		void* getRaw() const { return m_Data; };

	private:
		void* m_Data;
};

#endif // VARIANT_HPP
