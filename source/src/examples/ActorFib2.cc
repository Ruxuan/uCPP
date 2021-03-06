//                              -*- Mode: C++ -*- 
// 
// uC++ Version 7.0.0, Copyright (C) Peter A. Buhr 2016
// 
// ActorFib2.cc -- 
// 
// Author           : Peter A. Buhr
// Created On       : Mon Dec 19 08:21:34 2016
// Last Modified By : Peter A. Buhr
// Last Modified On : Sat Jan  6 09:22:17 2018
// Update Count     : 7
//
// This  library is free  software; you  can redistribute  it and/or  modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software  Foundation; either  version 2.1 of  the License, or  (at your
// option) any later version.
// 
// This library is distributed in the  hope that it will be useful, but WITHOUT
// ANY  WARRANTY;  without even  the  implied  warranty  of MERCHANTABILITY  or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
// 
// You should  have received a  copy of the  GNU Lesser General  Public License
// along  with this library.
// 

#include <iostream>
using namespace std;
#include <uActor.h>

unsigned int uDefaultActorThreads() { return 1; }
unsigned int uDefaultActorProcessors() { return 0; }

struct NextMsg : public uActor::Message {} nextMsg;
struct FibMsg : public uActor::Message { long long int fib; } fibMsg;

_Actor Fib {
    long long int fn1, fn2;

    Allocation receive( Message &msg ) {
	Case( NextMsg, msg ) {
	    fibMsg.fib = 0; fn1 = fibMsg.fib;
	    become( &Fib::receive2 );
	    *msg.sender | fibMsg;
	} else Case( StopMsg, msg ) {
	    return Delete;
	} // Case
	return Nodelete;
    } // Fib::receive

    Allocation receive2( Message &msg ) {
	Case( NextMsg, msg ) {
	    fibMsg.fib = 1; fn2 = fn1; fn1 = fibMsg.fib;
	    become( &Fib::receive3 );
	    *msg.sender | fibMsg;
	} else Case( StopMsg, msg ) {
	    return Delete;
	} // Case
	return Nodelete;
    } // Fib::receive2

    Allocation receive3( Message &msg ) {
	Case( NextMsg, msg ) {
	    fibMsg.fib = fn1 + fn2; fn2 = fn1; fn1 = fibMsg.fib;
	    *msg.sender | fibMsg;
	} else Case( StopMsg, msg ) {
	    return Delete;
	} // Case
	return Nodelete;
    } // Fib::receive3
}; // Fib

int Times = 10;						// default values

_Actor Generator {
    int i = 0;
    Fib *fib;

    void preStart() {
	fib = new Fib;
	fib->tell( nextMsg, this );
    } // Generator::preStart

    Allocation receive( Message &msg ) {
	if ( i < Times ) {
	    Case( FibMsg, msg ) {
		cout << msg_t->fib << endl;
		fib->tell( nextMsg, this );
	    } // Case
	    i += 1;
	    return Nodelete;
	} else {
	    *fib | stopMsg;
	    return Delete;
	} // if
    } // Fib::receive
}; // Generator

int main( int argc, char *argv[] ) {
    try {
	switch ( argc ) {
	  case 2:
	    Times = stoi( argv[1] );
	    if ( Times < 1 ) throw 1;
	  case 1:					// use defaults
	    break;
	  default:
	    throw 1;
	} // switch
    } catch( ... ) {
	cout << "Usage: " << argv[0] << " [ numbers (> 0) ]" << endl;
	exit( 1 );
    } // try

    new Generator;
    uActor::stop();					// wait for all actors to terminate
} // main

// Local Variables: //
// compile-command: "u++-work -g -O2 -multi ActorFib2.cc" //
// End: //
