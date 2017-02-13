
using System;

namespace Wup
{
    internal class ObjectCreateException : Exception
    {
        public ObjectCreateException(Exception ex)
            : base("ObjectCreateException", ex)
        {
        }
    }
}