#ifndef SecDecUtil_deep_apply_hpp_included
#define SecDecUtil_deep_apply_hpp_included

#include <secdecutil/series.hpp>
#include <functional>
#include <vector>

/*!
 * Implement the function "deep_aply".
 *
 * "deep_apply" is a function that applies another function to a
 * nested structure of "std::vector" and "secdecutil::series",
 * e.g. "std::vector<secdecutil::series<...>".
 *
 * It is useful for operations on all sectors to all orders.
 *
 */

namespace secdecutil {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /*

     Applies func to each base_type element of the nest and returns a new nest containing the result
     func may modify the elements of the original nest

     */
    template<typename out_base_type, typename in_base_type, typename T>
    struct deep_apply_impl
    {
        using base_type = T;
        using new_type = out_base_type;
        T& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply()
        {
            return func(nest);
        };
        deep_apply_impl(T& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for std::vector
    template<typename out_base_type, typename in_base_type, typename T>
    struct deep_apply_impl<out_base_type,in_base_type,std::vector<T>>
    {
        using base_type = typename deep_apply_impl<out_base_type,in_base_type,T>::base_type;
        using new_type = std::vector<typename deep_apply_impl<out_base_type,in_base_type,T>::new_type>;
        std::vector<T>& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply()
        {
            new_type content;
            for ( auto& element : nest )
                content.push_back( deep_apply_impl<out_base_type,in_base_type,T>(element,func).apply() );
            return content;
        };
        deep_apply_impl(std::vector<T>& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for secdecutil::Series
    template<typename out_base_type, typename in_base_type, typename T>
    struct deep_apply_impl<out_base_type,in_base_type,secdecutil::Series<T>>
    {
        using base_type = typename deep_apply_impl<out_base_type,in_base_type,T>::base_type;
        using new_type = secdecutil::Series<typename deep_apply_impl<out_base_type,in_base_type,T>::new_type>;
        secdecutil::Series<T>& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply()
        {
            std::vector<typename deep_apply_impl<out_base_type,in_base_type,T>::new_type> content;
            for ( auto& element : nest )
                content.push_back( deep_apply_impl<out_base_type,in_base_type,T>(element,func).apply() );
            return secdecutil::Series<typename deep_apply_impl<out_base_type,in_base_type,T>::new_type>
            (
             nest.get_order_min(),
             nest.get_order_max(),
             content,
             nest.get_truncated_above()
             );
        };
        deep_apply_impl(secdecutil::Series<T>& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    template<typename out_base_type, typename in_base_type, typename T>
    auto deep_apply(T& nest, const std::function<out_base_type(in_base_type)>& func)
    -> decltype( deep_apply_impl<out_base_type,in_base_type,T>(nest, func).apply() )
    {
        return deep_apply_impl<out_base_type,in_base_type,T>(nest, func).apply();
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /*

     Applies func to each base_type element of the nest and returns a new nest containing the result
     func may not modify the elements of the original nest

     */
    template<typename out_base_type, typename in_base_type, typename T>
    struct const_deep_apply_impl
    {
        using base_type = T;
        using new_type = out_base_type;
        const T& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply() const
        {
            return func(nest);
        };
        const_deep_apply_impl(const T& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for std::vector
    template<typename out_base_type, typename in_base_type, typename T>
    struct const_deep_apply_impl<out_base_type,in_base_type,std::vector<T>>
    {
        using base_type = typename const_deep_apply_impl<out_base_type,in_base_type,T>::base_type;
        using new_type = std::vector<typename const_deep_apply_impl<out_base_type,in_base_type,T>::new_type>;
        const std::vector<T>& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply() const
        {
            new_type content;
            for ( const auto& element : nest )
                content.push_back( const_deep_apply_impl<out_base_type,in_base_type,T>(element,func).apply() );
            return content;
        };
        const_deep_apply_impl(const std::vector<T>& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for secdecutil::Series
    template<typename out_base_type, typename in_base_type, typename T>
    struct const_deep_apply_impl<out_base_type,in_base_type,secdecutil::Series<T>>
    {
        using base_type = typename const_deep_apply_impl<out_base_type,in_base_type,T>::base_type;
        using new_type = secdecutil::Series<typename const_deep_apply_impl<out_base_type,in_base_type,T>::new_type>;
        const secdecutil::Series<T>& nest;
        const std::function<out_base_type(in_base_type)>& func;
        new_type apply() const
        {
            std::vector<typename const_deep_apply_impl<out_base_type,in_base_type,T>::new_type> content;
            for ( const auto& element : nest )
                content.push_back( const_deep_apply_impl<out_base_type,in_base_type,T>(element,func).apply() );
            return secdecutil::Series<typename const_deep_apply_impl<out_base_type,in_base_type,T>::new_type>
            (
             nest.get_order_min(),
             nest.get_order_max(),
             content,
             nest.get_truncated_above()
             );
        };
        const_deep_apply_impl(const secdecutil::Series<T>& nest, const std::function<out_base_type(in_base_type)>& func): nest(nest), func(func) {};
    };

    template<typename out_base_type, typename in_base_type, typename T>
    auto deep_apply(const T& nest, const std::function<out_base_type(in_base_type)>& func)
    -> decltype( const_deep_apply_impl<out_base_type,in_base_type,T>(nest, func).apply() )
    {
        return const_deep_apply_impl<out_base_type,in_base_type,T>(nest, func).apply();
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /*

     Applies func to each base_type element of the nest
     func may modify the elements of nest

     */
    template<typename in_base_type, typename T>
    struct void_deep_apply_impl
    {
        T& nest;
        const std::function<void(in_base_type)>& func;
        void apply()
        {
            func(nest);
        };
        void_deep_apply_impl(T& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for std::vector
    template<typename in_base_type, typename T>
    struct void_deep_apply_impl<in_base_type,std::vector<T>>
    {
        std::vector<T>& nest;
        const std::function<void(in_base_type)>& func;
        void apply()
        {
            for ( auto& element : nest )
                void_deep_apply_impl<in_base_type,T>(element,func).apply();
        };
        void_deep_apply_impl(std::vector<T>& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for secdecutil::Series
    template<typename in_base_type, typename T>
    struct void_deep_apply_impl<in_base_type,secdecutil::Series<T>>
    {
        secdecutil::Series<T>& nest;
        const std::function<void(in_base_type)>& func;
        void apply()
        {
            for ( auto& element : nest )
                void_deep_apply_impl<in_base_type,T>(element,func).apply();
        };
        void_deep_apply_impl(secdecutil::Series<T>& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    template<typename in_base_type, typename T>
    void deep_apply(T& nest, const std::function<void(in_base_type)>& func)
    {
        void_deep_apply_impl<in_base_type,T>(nest, func).apply();
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /*

     Applies func to each base_type element of the nest
     func may not modify the elements of nest (side-effects only)

     */
    template<typename in_base_type, typename T>
    struct void_const_deep_apply_impl
    {
        const T& nest;
        const std::function<void(in_base_type)>& func;
        void apply() const
        {
            func(nest);
        };
        void_const_deep_apply_impl(const T& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for std::vector
    template<typename in_base_type, typename T>
    struct void_const_deep_apply_impl<in_base_type,std::vector<T>>
    {
        const std::vector<T>& nest;
        const std::function<void(in_base_type)>& func;
        void apply() const
        {
            for ( auto& element : nest )
                void_const_deep_apply_impl<in_base_type,T>(element,func).apply();
        };
        void_const_deep_apply_impl(const std::vector<T>& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    // Specialisation for secdecutil::Series
    template<typename in_base_type, typename T>
    struct void_const_deep_apply_impl<in_base_type,secdecutil::Series<T>>
    {
        const secdecutil::Series<T>& nest;
        const std::function<void(in_base_type)>& func;
        void apply() const
        {
            for ( auto& element : nest )
                void_const_deep_apply_impl<in_base_type,T>(element,func).apply();
        };
        void_const_deep_apply_impl(const secdecutil::Series<T>& nest, const std::function<void(in_base_type)>& func): nest(nest), func(func) {};
    };

    template<typename in_base_type, typename T>
    void deep_apply(const T& nest, const std::function<void(in_base_type)>& func)
    {
        void_const_deep_apply_impl<in_base_type,T>(nest, func).apply();
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif
