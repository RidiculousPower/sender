require_relative '../sender/lib/sender/sender'

require 'pp'

class Test
  
  def initialize
    puts 'In method <Test>:initialize'
    puts 'Sender was: ' + __sender__.pretty_inspect.to_s
    puts 'Caller was: ' + __caller__.to_s
  end
  
  def test
    puts 'In <Test>:test'
    self.another_test
  end
  
  def another_test
    puts 'In method <Test>:another_test'
    test2 = Test2.new
    test2.and_another_test_in_another_object
  end
    
  def and_another_test_in_another_object
    puts 'In method <Test>:and_another_test_in_another_object'
    puts 'Sender was: ' + __sender__.pretty_inspect.to_s
    puts 'Caller was: ' + __caller__.to_s
  end
    
end

class Test2 < Test
  
  def initialize
    puts 'In method <Test2>:initialize'
    super
    puts 'Sender was: ' + __sender__.pretty_inspect.to_s
    puts 'Caller was: ' + __caller__.to_s
    pp Kernel.backtrace
  end
  
  def and_another_test_in_another_object
    puts 'In method <Test2>:and_another_test_in_another_object'
    super
    pp self
    puts 'Sender was: ' + __sender__.pretty_inspect.to_s
    puts 'Caller was: ' + __caller__.to_s
    pp Kernel.backtrace    
    pp Kernel.backtrace( 2 )
    puts 'These should be true:'
    pp Kernel.backtrace_includes?( :another_test )
    pp Kernel.backtrace_includes?( Test )
    pp Kernel.backtrace_includes?( $test )
    pp Kernel.backtrace_includes?( :another_test, Test, $test )
    pp Kernel.backtrace_includes?( "sender_test.rb" )
    puts 'These should be false:'
    pp Kernel.backtrace_includes?( :yet_another_test )
    pp Kernel.backtrace_includes?( Test2 )
    pp Kernel.backtrace_includes?( self )
    pp Kernel.backtrace_includes?( :yet_another_test, Test2, self )
    pp Kernel.backtrace_includes?( "sender_test.rbi" )
    
    puts 'And now we get a step by step backtrace'
    which_step = 1
    Kernel.each_backtrace_frame do |this_frame|
      puts 'Frame number ' + which_step.to_s + ':'
      pp this_frame
      which_step += 1
    end
    puts 'And now we try a backtrace inside a block.'
    block_item = [ 'one_item' ]
    block_item.each do |this_item|
      pp Kernel.backtrace
    end
    
    puts 'And :backtrace_includes_one_of?; this should be true:'
    pp Kernel.backtrace_includes_one_of?( :some_method_that_does_not_exit, :another_test, :test, :some_other_test_that_does_not_exist )
    puts 'as should this:'
    pp Kernel.backtrace_includes_one_of?( { :method => :another_test, :object => $test }, { :method => :test } )

    puts 'And :backtrace_frame_with; this should be a Hash:'
    pp Kernel.backtrace_frame_with( :test )
    puts 'as should this:'
    pp Kernel.backtrace_frame_with( "sender_test.rb" )

    puts 'And :backtrace_frames_with; this should be an Array of Hashes'
    pp Kernel.backtrace_frames_with( :object => $test )
    puts 'as should this:'
    pp Kernel.backtrace_frames_with( :file => "sender_test.rb" )

    puts 'And try iterating with an Enumerator'
    enumerator = Kernel.each_backtrace_frame
    pp enumerator
    while result = enumerator.next
      pp result
    end

  end
  
end

$test = Test.new
$test.test

puts 'Finished Test.'
exit