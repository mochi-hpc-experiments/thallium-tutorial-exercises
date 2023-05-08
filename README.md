# Thallium tutorial exercises

This repository contains exercises meant to accompany Mochi tutorials.
These tutorials focus on using the C++ language, with the Thallium library.
They revolve around the development of a phone book microservice, i.e.
a service that associates names (`std::string`s) with phone
numbers (which will be represented as `uint64_t` values).
Only the first exercise uses the code present in this repository,
however this README provides instructions for all three exercises.
An equivalent repository for C projects is available
[here](https://github.com/mochi-hpc-experiments/margo-tutorial-exercises).

## Initial setup

Please follow the initial setup
[here](https://github.com/mochi-hpc-experiments/margo-tutorial-exercises), then
clone the present repository.

## Exercise 1: simple RPC and RDMA using Thallium

The *src* directory provides a *client.cpp* client code, a *server.cpp*
server code, a *types.hpp* header defining some types, and a *phonebook.hpp*.
file containing an implementation of a phonebook using an `std::unordered_map`.

In this exercise we will make the server manage a phonebook and
service two kinds of RPCs: adding a new entry, and looking up a phone
number associated with a name.

* Let's start by setting up the spack environment and building the existing
code:

```
$ spack env create tl-tuto-env spack.yaml
$ spack env activate tl-tuto-env
$ spack install
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This will create the *client* and *server* programs.

* You can test your client and server programs by opening two terminals
  (make sure you have run `spack env activate tl-tuto-env` in them to
  activate your spack environment) and running the following from the *build*
  directory.

  For the server:

```
$ src/server na+sm
```

  This will start the server and print its address. `na+sm` (the shared memory
  transport) may be changed to `tcp` if you run this code on multiple machines
  connected via an Ethernet network.

  For the client:

```
$ src/client na+sm <server-address>
```

  Copying `<server-address>` from the standard output of the server command.
  The server is setup to run indefinitely. You may kill it with Ctrl-C.

* Looking at the API in *phonebook.hpp*, edit *server.cpp* to instanciate
  a phonebook in `main()`.

* Our two RPCs, which we will call "insert" and "lookup", will need
  argument and return types. Edit the *types.hpp* file to add the necessary
  type definitions for these RPCs, for instance an `entry` class containing
  a string field and a `uint64_t` field.

* Edit *server.cpp* to add the definitions and declarations of the lambda functions
  for our two RPCs. Feel free to copy/paste and modify the existing `sum` RPC.

* Edit *client.cpp* and use the existing code as an example to (1) register
  the two RPCs here as well, and (2) define two `insert` and `lookup` functions
  that will take a `tl::engine` alongside the necessary arguments
  to call the functions on the server.

Note: you may need to include `thallium/serialization/stl/string.hpp` in `types.hpp`
so that Thallium knows how to serialize strings.

* Try out your code by calling these `insert` and `lookup` functions a few
  times in the client.

### Bonus

Do this bonus part only if you have time, or as an exercise later.
In this part, we will add a `lookup_multi` RPC that uses
RDMA to send multiple names at once and return the array of associated phone
numbers(in practice this would be too little data to call for the use of RDMA,
but we will just pretent). For this, you may use the example
[here](https://mochi.readthedocs.io/en/latest/thallium/08_rdma.html).

* Your `lookup_multi` RPC could take the list of names to look up as a `vector` of
  `string`s and return a vector of `uint64_t`s.

* You will need to create two bulk handles on the client and two on the server.
  On the client, the first will expose the names as read-only (remember that
  `engine::expose` can take a vector of non-contiguous segments, but you will
  need to use `name.size()+1` as the size of each segment to keep the null
  terminator of each name), and the second will expose the output array as write only.

* You will need to transfer the two bulk handles in the RPC arguments,
  and since names can have a varying size, you will have to also transfer the
  total size of the bulk handle wrapping names, so that the server knows
  how much memory to allocate for its local buffer.

* On the server side, you will need to allocate two buffers; one to receive
  the names via a pull operation, the other to send the phone numbers via a push.

* You will need to create two `bulk` instances to expose these buffers.

* After having transferred the names, they will be in the server's contiguous
  buffers. You can rely on the null-terminators to know where one name ends and
  the next starts.


## Exercise 2: A proper phonebook Mochi component

In this exercise, we will use the Thallium microservice
[template](https://github.com/mochi-hpc/thallium-microservice-template/) to
develop a proper phonebook microservice.

* Click on the green "Use this template" button and select
  "Create a new repository". Give the repository a name (e.g. "phonebook").
  Put the repository on "private" if you wish, then click on
  "Create repository from template".

* Click on Settings > Actions > General, and set the Workflow permissions to
  "Read and write permissions", then Save.

* Go back to the root of the code (in your browser), and edit
  `initial-setup.json`. Change "alpha" to a service name of your choosing (this
  name will also be used to prefix your API functions, so choose something
  short, e.g. YP, for yellow pages. In the following, we will assume this is
  the name you used). Change "resource" to the name of the resource we are
  going to manage, here "phonebook". Click on the green "Commit changes"
  button.

* Wait a little and refresh the page. A GitHub workflow will have run and setup
  your code. _Note: other github workflows will run to test your code and
  upload a coverage report to codecov.io whenever you push commits to GitHub.
  These workflows will not work properly if you have made the repository
  private, so you may receive emails from GitHub about some failed workflows.
  Simply ignore them._

* Clone your code in the docker container, then create a Spack environment
  and build the code like you did in Exercise 1 by using the spack.yaml file at
  the root of your new project. Create a build directory and build the code.
  You may want to use the flag `-DENABLE_TESTS=ON` when calling `cmake` to make
  sure that the tests are also built.

It is now time to edit the code to make it do what we want. The template
provides the implementation of two RPCs, "hello" and "sum". These RPCs are
defined in a way that is very different from Exercise 1. Instead of using
lambdas, they are defined as member functions of the `ProviderImpl` class in
*src/ProviderImpl.hpp*.

Note: the architecture of this project uses the
[PImpl](https://en.cppreference.com/w/cpp/language/pimpl) technique, where
user-facing classes (e.g. `Client`, `PhonebookHandle`, etc.) only have a pointer
field along with public member functions, and the actual data associated with
an instance is in a private class (e.g. `ClientImpl`, `PhonebookHandleImpl`).
While this technique adds a small overhead in function calls, it also better
decouples the API of your service from its actual implementation.

* *include/YP/Client.hpp* contains the `Client` class, which will be used to
  register the RPCs and contact servers. There is nothing to modify in this file.

* *include/YP/PhonebookHandle.hpp* declares a `PhonebookHandle` class, which
  represents a phonebook managed by a remote server. This file is also where
  the relevant client interface will be defined. Go ahead and add declarations
  for the `insert` and a `lookup` member functions.

* *src/ClientImpl.hpp* contains the definition of the `ClientImpl` structure,
  which hides the actual data associated with a `Client` instance.
  Go ahead and add two `tl::remote_procedure` fields to represent the insert
  and lookup RPCs, and initialize them in the constructor.

* *src/ResourceHandle.cpp* contains the implementation of the `ResourceHandle`'s
  member functions. Go ahead and implement the `insert` and `lookup` functions.
  You can copy the `computeSum` function and adapt it accordingly.

Note: the template proposes that all the RPCs wrap their result in a
`RequestResult<T>` instance. You can view this class like a variant that will
either store a `T` instance or an error string if the operation failed. If you
implement your own service, feel free to handle errors differently.

Note: the template handles non-blocking RPCs by adding an optional
`AsyncRequest` pointer to each member function. Feel free to remove it (along
with the code path that deals with it) from your `insert` and `lookup`
functions. You can circle back to them later if you want, as an exercise.

At this point, feel free to compile your code to make sure it builds fine
You won't be able to test it yet since there is no server-side implementation
of our RPCs, so let's focus on the server library next.

* *include/YP/Backend.hpp* contains the definition of a backend, i.e. the
  abstract class that a phonebook implementation must inherit from. Add the
  proper `insert` and `lookup` pure virtual methods to this structure, following
  the model of the `computeSum` function.

* *src/dummy/DummyBackend.hpp* and *src/dummy/DummyBackend.cpp* contain a
  "dummy" implementation of such a backend in the form of a `DummyPhonebook`
  class. Add an `std::unordered_map<std::string,uint64_t>` field to this class,
  as well as the `insert` and `lookup` functions and their implementation.

* *src/ProviderImpl.hpp* contains the implementation of our provider. While it
  still follows the Pimpl idiom, with the `Provider` class containing a pointer
  to a `ProviderImpl` instance, you will notice that RPC functions are actually
  defined as member functions of the `ProviderImpl` class. This is because in
  Thallium, providers can expose their own member functions as RPC.
  Go ahead and add the two `tl::remote_procedure` fields for your insert and
  lookup RPCs. Don't forget to add their initialization in the constructor and
  to deregister them in the destructor!

* Still in *src/ProviderImpl.hpp*, implement the two `insertRPC` and `lookupRPC`
  member functions by taking example on the `computeSumRPC` member function.

At this point, you can make sure your code builds fine. Your microservice is
ready! If you have time, feel free to look into the *tests* folder, in
particular the *ResourceTest.cpp* file, and edit it (replacing calls to the
"computeSum" RPC) to try out your new functionalities.

In practice, the next steps at this point would be to (1) add more tests,
(2) remove everything related to the "hello" and "sum" RPCs (because obviously
a phonebook is not a calculator), and (3) implement more complex backends
by copying the code of the "dummy" backend and changing it to use external
libraries or more complicated implementations.


## Exercise 3: Using Bedrock and composing with other services

In this exercise we will use Bedrock to deploy a daemon containing
an instance of our phonebook provider. We will then implement a phonebook
backend that uses Yokan, and organize the composition of the two within
the same daemon. Everything in this exercise relies on the codebase from
Ex 2.

* First, make sure that the Spack environment from Ex 2 is activated.

* From the build directory, re-run cmake as follows.

```
$ cmake .. -DENABLE_TESTS=ON -DENABLE_BEDROCK=ON
$ make
```

This time a *libYP-bedrock-module.so* is being built. This is the Bedrock
module for our phonebook service, i.e. the library that tells Bedrock how
to instanciate and use our phonebook provider.

* To make sure Bedrock finds this library, execute the following command
  from the build directory.

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/src
```

* *examples/bedrock-config.json* is an example of Bedrock configuration that
  spins up a phonebook provider with provider ID 42. This provider manages
  one phonebook of type "dummy". You can try our this configuration using
  the `bedrock` program as follows.

```
$ bedrock na+sm -c ../examples/bedrock-config.json
```

* You can copy the address printed by bedrock on the last line of its log,
  and in another terminal (don't forget to activate your spack environment),
  run the following command.

```
$ bedrock-query na+sm -a <copied-address> -p
```

You will see the current configuration of the service, including a
phonebook provider that manages a phonebook. Bedrock has completed the
input configuration with a lot of information about Mercury, Argobots,
etc. These information can be very useful to communicate to Mochi developers
when you try to find out what's wrong with your service.

We will now add Yokan in our service.

* To add Yokan as dependency to our spack environment, run the following
  command.

```
$ spack add mochi-yokan+bedrock
$ spack install
```

This will install Yokan. You can also edit *spack.yaml* at the root of your
project to add `mochi-yokan+bedrock` in the list of specs.

* Edit *CMakeLists.txt* to add `find_package(yokan REQUIRED)` (e.g. after
  the call to `find_package(thallium REQUIRED)`).
  _Note: when developing your own service, don't forget to also edit the
  `src/*.cmake.in` and `src/*.pc.in` files  to add relevant dependencies
  there. Those are the files used by cmake and pkg-config respectively to
  search for dependencies when people are using your code._

* Edit *src/CMakeLists.txt* to add yokan-client as a dependency for the
  YP-server library (i.e. find the call to `target_link_libraries` for
  YP-server and add yokan-client in the list of public dependencies).

* From the build directory, re-run `cmake ..` to make it find Yokan.

* Open *examples/bedrock-config.json* and add the Yokan library in the
  `libraries` section.

```json
"yokan": "libyokan-bedrock-module.so"
```

* In this file as well, we will instanciate a Yokan provider with
  a Yokan database. In the `providers` section, *before* the phonebook provider,
  add the following provider definition.

```json
{
  "type": "yokan",
  "name": "my-yokan-provider",
  "provider_id": 123,
  "config": {
    "databases": [
      {
        "type": "map",
        "name": "my-db"
      }
    ]
  }
},
```

If you re-run bedrock with this new configuration then call bedrock-query,
you should be able to confirm that your Bedrock daemon is now running two
providers: one YP provider and one Yokan provider. Of course, these two
don't know about each other, they simply share the resources of the same
process. We will now introduce a dependency between YP and Yokan.

* Edit *src/BedrockModule.cpp* and find the `getProviderDependencies`
  member function at the end. Change
  `static const std::vector<bedrock::Dependency> no_dependency;` into a variable
  that lists en actual dependency, i.e.:

```cpp
static const std::vector<bedrock::Dependency> dependencies =
    {{"yokan_ph", "yokan", BEDROCK_REQUIRED}};
```

The first field, `"yokan_ph"`, is the name by which YP will reference
this dependency. `"yokan"` is the type of dependency. `BEDROCK_REQUIRED`
indicates that this dependency is required.

* If you rebuild your code now and re-run the Bedrock configuration,
  it will display an error message:

```
[critical] Missing dependency yokan_ph in configuration
```

So let's fix that by going again into *examples/bedrock-config.json*,
and add the following in the field in the definition of our YP provider.

```json
"dependencies": {
  "yokan_ph": "yokan:123@local"
}
```

You can also use `"my-yokan-provider"` instead of `"yokan:123"`.
Now Bedrock should restart accepting your configuration.

* In *src/ProviderImpl.hpp* add a `const tl::provider_handle& yokan_ph`
  argument to the constructor. Add a corresponding `m_yokan_ph` field to the
  class and assign the provided constructor argument to it.

* In *include/YP/Provier.hpp*, add a `const tl::provider_handle& yokan_ph`
  argument to the two constructors. In *src/Provider.cpp* change the signature
  of the constructor accordingly as well as the call to the underlying
  ProviderImpl constructor.

* Edit *src/BedrockModule.cpp* once again. This time we will look at the
  `registerProvider` function at the beginning of the file.
  use the `args` variable to find the dependency to Yokan as follows.

```cpp
yk_provider_handle_t yokan_ph =
  args.dependencies["yokan_ph"]
      .dependencies[0]->getHandle<yoken_provider_handle_t>();
```

You can convert this `yokan_ph` into a thallium provider handle as follows
before passing it to the the `Provider` constructor.

```cpp
tl::provider_handle ph{
    args.engine, yokan_ph->addr, yokan_ph->provider_id, false};
```

You have successfully injected a Yokan dependency into the YP provider!

The rest of this exercise will be less directed. The goal is now to pass this
provider handle down to the dummy phonebook so that it can use Yokan as an
implementation of a key/value store instead of relying on an unordered_map.
You should now be familiar enough with the code to make the necessary changes
bellow without too much guidance. Keep the API of Yokan open in a web browser
for reference. You can find it [here](https://github.com/mochi-hpc/mochi-yokan).
Yokan also has a C++ API
[here](https://github.com/mochi-hpc/mochi-yokan/blob/main/include/yokan/cxx/).

* To be able to pass the Yokan provider handle down to a backend (e.g. a dummy
  phonebook), you will need to change the signature of the functions that
  create and open a phonebook (the `createPhonebook` and `openPhonebook` in
  *include/YP/Backend.hpp*, as well as the type of `std::function` stored
  in `create_fn` and `open_fn` maps, and the signatures of the lambdas in the
  `__PhonebookBackendRegistration` class down the file).

* This then implies changing *src/dummy/DummyBackend.cpp* and
  *src/dummy/DummyBackend.hpp* accordingly.

* You will need to tell your dummy phonebook backend which database to use.
  Yokan databases can be identified by a name, so you may want to implement a
  way to look for the name of this database in the configuration passed to the
  phonebook.

* Once a backend knows the name of the database it should use, you can use
  `yokan::Client::findDatabaseByName` to look for the database and obtain a
  `yokan::Database` instance that you can store in the `DummyBackend` class.

* In the `insert` and `lookup` functions of the dummy phonebook, you may now
  use the `put` and `get` methods of this `yokan::Database` instance to put
  and get phone numbers.

In practice, you could copy the dummy backend implementation into a new
type of backend that specifically uses Yokan. Don't hesitate to implement
multiple backends for your service, with different dependencies or different
strategies for solving the same problem.
