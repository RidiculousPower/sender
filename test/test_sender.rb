require "test/unit"
require "sender"

class TestSender < Test::Unit::TestCase

  def test___sender__
    assert_equal self, __sender__
  end

  def test___caller__
    assert_equal :run, __caller__
  end

  def test_Kernel_backtrace
    # not sure how to test this
  end
  
  def test_Kernel_backtrace_includes
    # not sure how to test this    
  end

end
