\page git_manual Git Configuration Manual

\section git_manual_signed_commits Signed Commits

Git users have the option to digitally sign commits prior to pushing them to a repository. This procedure ensures the authenticity of the contributor by verifying that the individual submitting the commits is indeed the authorized author. To enable commit signing, the installation of GPG is required.

**Step 1 – Install GPG**  

**Windows**  
**- MINGW64 Console**  
```
pacman -S mingw-w64-x86_64-gnupg
```

**- PowerShell**  
```
winget install --id GnuPG.GnuPG
```

**Linux**  
```
sudo apt install gnupg
```

---  
Once the installation is complete, **verify that GPG has been installed correctly** by executing:
```
gpg --version
```
---

The configuration of GPG keys with Git and GitHub can be accomplished by following these steps:

**Step 2 – Generate an RSA Key**  
```
gpg --full-generate-key
```
**Type**: RSA  
**Key Size**: 4096 bits  
**Expiration**: 1 year  
**Email**: Use the email address associated with your Git configuration. Verify your email using `git config user.email`.

To review your keys, execute:  
```
gpg --list-secret-keys --keyid-format=long
```

The output should resemble the following:  
```
/home/user/.gnupg/secring.gpg
sec rsa4096/ABCDEF1234567890 2025-09-22 [SC]
1234567890ABCDEF1234567890ABCDEF12345678
uid [ultimate] Your Name your-email@example.com
ssb rsa4096/0987654321FEDCBA 2025-09-22 [E]
```

**Step 3 – Export the Public Key**  
```
gpg --armor --export ABCDEF1234567890 > key.txt
```

Note that `ABCDEF1234567890` corresponds to the key ID displayed when listing the keys in the previous step.

This command generates a TXT file containing the public key, which should appear as follows:  
```
-----BEGIN PGP PUBLIC KEY BLOCK-----
...
-----END PGP PUBLIC KEY BLOCK-----
```

**Step 4 – Add the GPG Key to GitHub**  
1. Navigate to **GitHub** → **Settings** → **SSH and GPG keys** → **New GPG key**.  
2. Paste the entire **ASCII-armored public key** into the designated field.  
3. Click **Add GPG key** to complete the process.

\section git_manual_git_cli Git CLI (For Benchmark)

**Step 1: Installation**
**- Windows - MINGW64 Console**
Ensure that you are running the MINGW64 terminal and not the MSYS or MINGW32 terminal to avoid compatibility issues.
```
pacman -S mingw-w64-x86_64-gh
```

**- Linux**
```
sudo apt install gh
```

Once the installation is complete, **verify that GitHub CLI has been installed correctly** by executing:
```
gh --version
```

**Step 2: Authentication**

In order to use the commands provided by GitHub CLI, it is necessary to **authenticate**:

```
gh auth login
```

During this process, you will be prompted to:
1. Select the GitHub host (`GitHub.com` or `GitHub Enterprise`).
2. Choose the preferred protocol for repositories (`HTTPS` or `SSH`).
3. Authenticate via a web browser or a `personal access token (PAT)`.

After completing the authentication process, **verify the authentication status** using:

```
gh auth status
```
This command confirms that your account is correctly linked and shows which authentication method is being used.