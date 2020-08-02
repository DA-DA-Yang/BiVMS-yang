/*********************************************************************
 This file is part of VDM, an app. for visual deformation measurement.
		   Copyright(C) 2019-now, SHU, all rights reserved.
*********************************************************************/
#pragma once
#include <type_traits>
#include <xmemory>

#define CONNECT_TO_THIS(OBJ, SIG, EXP) \
connect(OBJ, &std::conditional_t< \
std::is_pointer_v<decltype(OBJ)>, \
std::pointer_traits<decltype(OBJ)>::element_type, \
std::decay_t<decltype(OBJ)>>::SIG, \
this, &std::pointer_traits<decltype(this)>::element_type::EXP);

#define CONNECT_BETWEEN(FROM, SIG, TO, SLT) \
connect(FROM, &std::conditional_t< \
std::is_pointer_v<decltype(FROM)>, \
std::pointer_traits<decltype(FROM)>::element_type, \
std::decay_t<decltype(FROM)>>::SIG, \
TO, &std::conditional_t< \
std::is_pointer_v<decltype(TO)>, \
std::pointer_traits<decltype(TO)>::element_type, \
std::decay_t<decltype(TO)>>::SLT);

#define CONNECT_TO_LAMBDA(OBJ, SIG, EXP) \
connect(OBJ, &std::conditional_t< \
std::is_pointer_v<decltype(OBJ)>, \
std::pointer_traits<decltype(OBJ)>::element_type, \
std::decay_t<decltype(OBJ)>>::SIG, \
this, [=]{EXP;});

#define CONNECT_EXPLICITLY(FROM, SIG, TO, SLT) \
connect(FROM, SIGNAL(SIG), TO, SLOT(SLT));

#define MSG_WARNING(TEXT) \
QMessageBox::warning(this, tr("¾¯¸æ"), \
tr(TEXT), QMessageBox::Ok);

#define ELEMENTSIZE_CXP 0.0045;
#define ELEMENTSIZE_MER 0.00345;