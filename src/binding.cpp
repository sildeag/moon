/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * binding.cpp: data binding
 *
 * Contact:
 *   Moonlight List (moonlight-list@lists.ximian.com)
 *
 * Copyright 2008 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "binding.h"
#include "enums.h"

Binding::Binding ()
{
 	property_path = g_strdup ("");
 	binding_mode = BindingModeOneWay;
	notify_on_validation_error = false;
	validates_on_exceptions = false;
 	is_sealed = false;
}

Binding::~Binding ()
{
	g_free (property_path);
}

char *
Binding::GetPropertyPath ()
{
	return property_path;
}

void
Binding::SetPropertyPath (const char *path)
{
	g_free (property_path);
	property_path = g_strdup (path);
}

BindingMode
Binding::GetBindingMode ()
{
	return binding_mode;
}

void
Binding::SetBindingMode (BindingMode mode)
{
	binding_mode = mode;
}


BindingExpressionBase::BindingExpressionBase ()
{
	binding = NULL;
	got_value = false;
	gv_callback = NULL;
	source_name = NULL;
	source = NULL;
	stored_value = NULL;
	sv_callback = NULL;
	target = NULL;
	target_property = NULL;
	converter = NULL;
	culture = NULL;
	param = NULL;
}

BindingExpressionBase::~BindingExpressionBase ()
{
	if (binding)
		binding->unref ();
	
	if (stored_value)
		stored_value->FreeValue ();
	
	g_free (source_name);
	g_free (converter);
	g_free (culture);
	g_free (param);
}

void
BindingExpressionBase::SetBinding (Binding *binding)
{
	binding->ref ();
	if (this->binding)
		this->binding->unref ();
	this->binding = binding;
}

void
BindingExpressionBase::SetConverterParameter (const char *param)
{
	g_free (this->param);
	this->param = g_strdup (param);
}

void
BindingExpressionBase::SetConverterCulture (const char *culture)
{
	g_free (this->culture);
	this->culture = g_strdup (culture);
}

void
BindingExpressionBase::SetConverter (const char *converter)
{
	g_free (this->converter);
	this->converter = g_strdup (converter);
}

void
BindingExpressionBase::SetSourceName (const char *name)
{
	g_free (source_name);
	source_name = g_strdup (name);
}

void
BindingExpressionBase::SetSource (DependencyObject *source)
{
	// FIXME: do we want to ref the element? or at least listen for the destroy signal?
	this->source = source;
}

void
BindingExpressionBase::SetTarget (FrameworkElement *target)
{
	// We have to listen to property changes on the target
	this->target = target;
}

void
BindingExpressionBase::AttachListener (DependencyObject *target, PropertyChangeHandler handler, gpointer user_data)
{
	DependencyProperty *property;
	
	if (binding && binding->GetPropertyPath () && handler) {
		if (!source && source_name)
			source = target->FindName (source_name);
		
		if (source && (property = DependencyProperty::GetDependencyProperty (source->GetType ()->GetKind (), binding->GetPropertyPath ())))
			source->AddPropertyChangeHandler (property, handler, user_data);
	}
}

void
BindingExpressionBase::DetachListener (PropertyChangeHandler handler)
{
	DependencyProperty *property;
	
	if (source && binding && binding->GetPropertyPath () && handler) {
		if ((property = DependencyProperty::GetDependencyProperty (source->GetType ()->GetKind (), binding->GetPropertyPath ())))
			source->RemovePropertyChangeHandler (property, handler);
	}
}

Value *
BindingExpressionBase::GetValue ()
{
	if (!got_value || true) {
		got_value = true;
		
		if (stored_value) {
			stored_value->FreeValue ();
			stored_value = NULL;
		}
		
		if (gv_callback) {
			Value value = gv_callback ();
			if (value.GetKind () == Type::INVALID)
				stored_value = NULL;
			else
				stored_value = new Value (value);
			value.FreeValue ();
		} else if (binding && binding->GetPropertyPath ()) {
			DependencyProperty *property;
			
			if (!source && source_name && target)
				source = target->FindName (source_name);
			
			if (source && (property = DependencyProperty::GetDependencyProperty (source->GetType ()->GetKind (), binding->GetPropertyPath ())))
				stored_value = source->GetValue (property);
		}
	}
	
	return stored_value;
}

void
BindingExpressionBase::RegisterManagedOverrides (GetValueCallback gv_callback, SetValueCallback sv_callback)
{
	this->gv_callback = gv_callback;
	this->sv_callback = sv_callback;
}

void
BindingExpressionBase::UpdateSource (Value *value)
{
	DependencyProperty *property;
	
	if (sv_callback) {
		sv_callback (value);
	} else if (binding && binding->GetPropertyPath ()) {
		if (!source && source_name && target)
			source = target->FindName (source_name);
		
		if (source && (property = DependencyProperty::GetDependencyProperty (source->GetType ()->GetKind (), binding->GetPropertyPath ())))
			source->SetValue (property, value);
	}
}
