module PCGL
  module Statement
    # raw stub
    class Raw < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
