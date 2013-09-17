Execution
=========

How Ydin Executes Operations {#how_ydin_executes}
----------------------------

PiiEngine is completely unaware of any connections between the
operations that have been added to it with
[PiiEngine::addOperation()]. When a configuration is executed,
PiiEngine first checks whether the operations are ready to be executed
by calling [PiiOperation::check()] of each operation it currently
holds. If everything is fine, PiiEngine calls
[PiiOperation::execute()] of all the operations. It does not know what
happens, but is able to track the state of each operation in the
configuration. The engine stops running once all of its operations
have stopped either spontaneously, due to an error or because of an
explicit [interrupt](PiiEngine::interrupt()).

The configuration may stop either spontaneously or after a user
command. Spontaneous exit may happen, for example, if an image
source runs out of images. In such a case the image source sends a
signal that tells the next processing layer to also stop once they
have finished processing the last image. The signal is transmitted
to all connected operations, which causes the engine to finally
stop. Now, if the configuration has multiple processing pipelines
that are not connected to each other by forward paths, i.e. there
are multiple independent producers, the engine will not stop until
all producers have finished their execution.

A working configuration must have at least one producer operation. 
Emission of objects from this operation causes processing in
the others. When a producer emits an object through its output,
other operations connected to that output are signalled to start a
new processing round. Depending of the type of the receiving
operation, the processing may happen in parallel or in the same
execution thread with the sender. Parallel execution can happen in
any of the computer's processors, depending on the load of the
system.


Synchronization {#synchronization}
---------------

Although there is no clock signal that would synchronize
operations, all operations still work in sync with each other by
default. This is ensured by making each output socket send one
object for each one read from an input. Thus, in the case of
multiple input sockets, the operation waits until an object is
present in each and sends one and only one object to each output.

This method works as far as only one object is sent for each input
object. Things get complicated when an operation changes the rate
of objects by emitting either more or less objects. This is still
acceptable if the flows with different object rates are handled in
separate pipelines. Problems appear when these pipelines are
connected. One perfectly valid solution is to not allow such
connections. The engine itself does not care about synchronization,
and you are free to implement the operations just as you like. If
you can remain asynchronous, you are probably faster than
synchronous. In many cases, however, the problem needs to be faced
an solved. The following sections describe the default
synchronization mechanism that is supported by all built-in
operations. You are free not to follow the flock and implement your
own system, but be aware of compatibility issues.


### Default Synchronization Mechanism ### {#synchronization_default}

@image html synchronization.png

A typical synchronization issue appears when a large image is split
into smaller sub-images, and an operation later in the processing
pathway needs to merge the sub-images originating from a single
large image, as shown in the figure above. Given no synchronization
information, the merging operation (PiiImagePieceJoiner) has no way
of knowing how many `locations` and `labels` are received for
every `image`.

The problem could be solved by forcing all operations to emit
exactly one object for each object they read. In such a scenario,
the sub-images should be packed into an object that represents
multiple images. The drawback is that all operations should be
aware of a new data type: a list of images. Furthermore, they
should also know a list of lists, a list of lists of lists etc. An
even more severe drawback would be that all the sub-images should
be processed in a batch, and one could not utilize parallel
computation in an efficient manner.

The solution adopted in Ydin is that each operation indicates the
input socket (if any) an output socket it is synchronized to. For
example, the outputs of an [image splitter](PiiImageSplitter)
are synchronized to its inputs because image pieces will only be
emitted if an image is received in the `image` input. Given this
information, Ydin is able to detect, for example, when all the
sub-images originating from a single large image have been received
by an operation. Another requirement is that an operation that
breaks the "normal" 1:1 input-output relationship on synchronized
sockets informs the system whenever it misbehaves. Please refer to
[configuring_synchronization] for detailed instructions.

### Synchronization Example ### {#synchronization_example}

Let us go through a short example to illustrate how the
synchronization technique really works. In the figure above,
PiiImageFileReader emits an image through its `image` output whenever
it has decoded a file. The image goes to the `image` input of
PiiImagePieceJoiner. In PiiImagePieceJoiner, `label` and `location`
inputs are grouped and form a synchronized socket group. The `image`
input has no synchronized pair. Therefore, the operation does not need
to wait for more objects but can process the image immediately.

The image also goes to the `image` input of PiiImageSplitter.  Since
the operation has no other inputs, it is processed right away. Before
the operation actually sends something to its outputs, it calls
PiiOutputSocket::startMany() to indicate that it may send more than
one object. This causes a synchronization tag to be emitted through
the `subimage` and `location` outputs. It is subsequently received by
`CustomClassifier`, which has only one input. The tag will be accepted
and passed to the `label` output.

Now there is a synchronization tag in the `label` input of
PiiImagePieceJoiner. Since `label` and `location` are in the same
synchronization group, the operation needs to wait. But not too long,
because a similar tag was just sent from the `location` output of
PiiImageSplitter. Depending on how `CustomClassifier` is implemented
it may happen that the tag appears first in the `location` input of
PiiImagePieceJoiner, but this does not matter.  Once both are there,
the operation passes the tag to all outputs synchronized to that group
of inputs. It also marks the *flow* *level* of the inputs to be one
higher than before and sends a [synchronization
event](PiiFlowProcessor::Listener::syncEvent()).

Now that the synchronization tags are processed PiiImageSplitter
starts sending actual objects. `CustomClassifier` will be processed
right after receiving an image, and PiiImagePieceJoiner every time
both `label` and `location` are there. Finally, PiiImageSplitter stops
splitting the image and calls PiiOutputSocket::endMany(). This will
send another synchronization tag, which tells the operations later in
the pipeline to lower their flow level. PiiImagePieceJoiner notices
that the flow level of the `label` and `location` inputs is now equal
to that of the `image` input and sends another synchronization event.

Meanwhile, PiiImageFileReader has been busy decoding a new image.  It
may have already completed its job and sent the image to
PiiImageSplitter and PiiImagePieceJoiner. It may even be fetching the
third image, but it cannot send it because the inputs have not been
cleared yet. Anyhow, the image is there again, or at least it will be
there in a while, and the process starts from the beginning.

### Stopping and Pausing ### {#synchronization_stop_pause}

The execution of an engine can stop spontaneously. In the example
above, PiiImageFileReader sends a *stop tag* when it runs out of
images. Whenever an operation receives such a tag in all of its
inputs, it will finish its execution. If the configuration has many
independent producers, all of them must stop before the whole engine
changes its state to `Stopped`.

[Pausing](PiiOperation::pause()) and [non-spontaneous
stopping](PiiOperation::stop()) work in a similar manner, but they are
initiated outside of the operation. When an operation with no
connected inputs receives a pause or stop signal, it will turn into
`Paused` or `Stopped` state whenever it is done with its current
processing round. It subsequently send a *pause tag* or a stop `tag
which, when received into all inputs of an operation, turns that
operation into `Paused` or `Stopped` state. This will finally pause or
stop the whole processing pipeline.
