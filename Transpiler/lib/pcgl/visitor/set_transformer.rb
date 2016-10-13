module PCGL
  module Visitor
    # transforms the assignment
    #     set x, y
    # to
    #     x = y
    # will probably be deleted in future
    class SetTransformer < Base
      def transform_command(object)
        if object.meta[:name] == 'set'
          ret = Statement::Assignment.new('')
          ret.meta = {
            left: object.meta[:parameter].first,
            right: object.meta[:parameter].last
          }
          ret
        else
          object
        end
      end
    end
  end
end
