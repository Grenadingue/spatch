#ifndef		USER_HPP_
# define	USER_HPP_

class User
{

public:
  User();
  User(const User&);
  ~User();
  User &operator=(const User&);
};

#endif		/* !USER_HPP_ */
