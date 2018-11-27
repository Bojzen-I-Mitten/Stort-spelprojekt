using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using Xceed.Wpf.Toolkit.PropertyGrid;

namespace ThomasEditor.utils
{
    public class ParentObjectCommand
        : ICommand
    {
        readonly GameObject parent;
        readonly GameObject child;
        readonly bool keepTransform;
        public ParentObjectCommand(GameObject child, GameObject parent)
        {
            this.parent = parent;
            this.child = child;
            this.keepTransform = true;
        }

        public void execute()
        {
            if(!parent)
                child.transform.SetParent(null, keepTransform);
            else
                child.transform.SetParent(parent.transform, keepTransform);
        }
    }
    public class EnableComponentCommand
        : ICommand
    {
        readonly Component comp;
        readonly bool enabled;
        public EnableComponentCommand(Component comp, bool enabled)
        {
            this.comp = comp;
            this.enabled = enabled;
        }

        public void execute()
        {
            comp.enabled = enabled;
            comp.Activated = enabled;
        }
    }
    public class AddComponentCommand
        : ICommand
    {
        GameObject obj;
        Type comp_type;
        public AddComponentCommand(GameObject obj, Type comp)
        {
            this.obj = obj;
            this.comp_type = comp;
        }

        public void execute()
        {
            var method = typeof(GameObject).GetMethod("AddComponent").MakeGenericMethod(comp_type);
            method.Invoke(obj, null);
        }
    }
    public class ComponentSetIndexCommand
        : ICommand
    {
        Component comp;
        uint index;
        public ComponentSetIndexCommand(Component comp, uint index)
        {
            this.comp = comp;
            this.index = index;
        }

        public void execute()
        {
            comp.gameObject.SetComponentIndex(comp, index);
        }
    }

    /* UI Thread: Nobody toucha my spagett!!!!!!!!!! ._,
     * 
    public class CPropertyObjectCommand<T>
        : ICommand
    {
        readonly T resource;
        readonly CustomPropertyItem prop;
        public CPropertyObjectCommand(T resource, CustomPropertyItem prop)
        {
            this.resource = resource;
            this.prop = prop;
        }

        public void execute()
        {
            prop.Value = resource;
        }
    }
    public class CPropertyListCommand<T>
        : ICommand
    {
        readonly T resource;
        readonly int insertAt;
        readonly System.Collections.IList list;
        public CPropertyListCommand(T resource, System.Collections.IList list, int insertAt)
        {
            this.resource = resource;
            this.insertAt = insertAt;
            this.list = list;
        }

        public void execute()
        {
            list[insertAt] = resource;
        }
    }
    public class PropertyCommand<T>
        : ICommand
    {
        readonly T resource;
        readonly PropertyItem pi;
        public PropertyCommand(T resource, PropertyItem pi)
        {
            this.resource = resource;
            this.pi = pi;
        }

        public void execute()
        {
            pi.Value = resource;
        }
    }
    public class PropertyListCommand<T>
        : ICommand
    {
        readonly T resource;
        readonly int insertAt;
        readonly System.Collections.IList list;
        public PropertyListCommand(T resource, int insertAt, System.Collections.IList list)
        {
            this.resource = resource;
            this.insertAt = insertAt;
            this.list = list;
        }

        public void execute()
        {
            list[insertAt] = resource;
        }
    }
    */
}
