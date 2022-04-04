#include <iostream>

#include <stdio.h>
#include <sys/errno.h>

#include <sys/acl.h>
#include <acl/libacl.h>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

#define PATHNAME "test"
uid_t get_id_obj(std::string str)
{
    struct stat buf;
    if (stat(PATHNAME, &buf) == -1)
    {
        perror("stat");
        return -1;
    }
    std::cout << str << buf.st_uid << "\n";
    return buf.st_uid;
}

int get_gr_name(gid_t id)
{
    struct group *gr_name = gr_name = getgrgid(id);
    if (gr_name == NULL)
    {
        perror("getgrgid");
        return -1;
    }
    std::cout << "group name: ";
    for (int i = 0; i < sizeof(gr_name->gr_name) / sizeof(gr_name->gr_name[0]); i++)
    {
        std::cout << gr_name->gr_name[i];
    }
    std::cout << "\n"
              << std::flush;
    return 0;
}

int analysis(acl_entry_t entry_p)
{
    acl_tag_t tag_type_p;
    int res = acl_get_tag_type(entry_p, &tag_type_p);
    if (res == -1)
    {
        perror("acl_get_tag_type");
        return -1;
    }
    std::cout << "Type: ";
    switch (tag_type_p)
    {
    case ACL_USER_OBJ:
    {
        std::cout << "ACL_USER_OBJ\n";
        if (get_id_obj("user_obj id: ") == -1)
        {
            std::cout << "stat problems\n";
            return -1;
        }
        break;
    }
    case ACL_USER:
    {
        std::cout << "ACL_USER; ";
        unsigned int *id = (unsigned int *)acl_get_qualifier(entry_p);
        if (id == NULL)
        {
            perror("acl_get_qualifier");
            return -1;
        }
        std::cout << "user id: " << *id << "; ";
        struct passwd *name = getpwuid((uid_t)(*id));
        if (name == NULL)
        {
            perror("getpwuid");
            return -1;
        }
        std::cout << "user name: ";
        for (int i = 0; i < sizeof(name->pw_name) / size_t(name->pw_name[0]); i++)
        {
            std::cout << name->pw_name[i];
        }
        acl_free((void *)id);
        // delete name;
        break;
    }
    case ACL_GROUP_OBJ:
    {
        std::cout << "ACL_GROUP_OBJ\n";
        int id = get_id_obj("group_obj id: ");
        if (id == -1)
        {
            std::cout << "stat problems\n";
            return -1;
        }
        get_gr_name(id);
        break;
    }
    case ACL_GROUP:
    {
        std::cout << "ACL_GROUP\n";
        unsigned int *id = (unsigned int *)acl_get_qualifier(entry_p);
        if (id == NULL)
        {
            perror("acl_get_qualifier");
            return 1;
        }
        std::cout << "group id: " << *id << "; ";

        get_gr_name((gid_t)(*id));

        acl_free((void *)id);
        break;
    }
    case ACL_MASK:
        std::cout << "ACL_MASK\n";
        break;
    case ACL_OTHER:
        std::cout << "ACL_OTHER\n";
        break;
    }

    acl_permset_t permset_p;
    res = acl_get_permset(entry_p, &permset_p);
    if (res == -1)
    {
        perror("acl_get_permset");
        return -1;
    }
    std::cout << "Access type: ";
        res = acl_get_perm(permset_p, ACL_READ);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_READ ";
    }

    res = acl_get_perm(permset_p, ACL_WRITE);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_WRITE ";
    }

    res = acl_get_perm(permset_p, ACL_EXECUTE);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_EXECUTE";
    }
    std::cout << "\n";
    return 0;
}

int main()
{
    std::cout << "Lab 9\n";
    acl_t list = acl_get_file(PATHNAME, ACL_TYPE_ACCESS);
    if (list == NULL)
    {
        perror("acl_get_file");
        exit(EXIT_FAILURE);
    }

    acl_entry_t entry_p;
    int res = 0;
    res = acl_get_entry(list, ACL_FIRST_ENTRY, &entry_p);
    if (res == -1)
    {
        perror("acl_get_entry");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while (true)
    {
        res = acl_get_entry(list, ACL_NEXT_ENTRY, &entry_p);
        if (res == -1)
        {
            perror("acl_get_entry");
            exit(EXIT_FAILURE);
        }
        if (res == 0)
        {
            break;
        }
        std::cout << "\n"
                  << count << "\n";
        analysis(entry_p);
        count++;
    }


    getchar();
    acl_entry_t entry_p;
    res = acl_create_entry(&list, &entry_p);
    if(res == -1)
    {
        perror("acl_create_entry");
        exit(EXIT_FAILURE);
    }

    res = acl_set_tag_type(entry_p, ACL_USER); // ACL_USER
    if(res == -1)
    {
        perror("acl_set_tag_type");
        exit(EXIT_FAILURE);
    }





    acl_free((void *)&list);
    return 0;
}